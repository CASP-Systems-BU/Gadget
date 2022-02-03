//
// Created by fl on 12/25/20.
//

#include "plg.h"

#include <experimental/filesystem>
#include "core/auto_ptr.h"
#include "core/faster.h"
//#include "device/null_disk.h"

#ifndef REPLAYER_PLG_FASTER_H
#define REPLAYER_PLG_FASTER_H

typedef FASTER::environment::QueueIoHandler handler_t;
/// Disk's log uses 64 MB segments.
typedef FASTER::device::FileSystemDisk<handler_t, 67108864> disk_t;    //32MB segments

//typedef FASTER::device::FileSystemDisk<handler_t, 268435456L> disk_t;
using namespace FASTER::core;

class plg_faster: public plg{
public:
    int opendb();
    int closedb();

    double opr_insert(uint64_t ikey, const char* val);
    double opr_update(uint64_t ikey, const char* val);
    double opr_read(uint64_t ikey);
    double opr_delete(uint64_t ikey);
    double opr_merge(uint64_t ikey, const char* val);

    class ReadContext;
    class UpsertContext;
    class RmwContext;

    Guid store_guid;

    /// This benchmark stores 8-byte keys in key-value store.
    class Key {
    public:
        Key(uint64_t key)
                : key_{ key } {
        }

        /// Methods and operators required by the (implicit) interface:
        inline static constexpr uint32_t size() {
            return static_cast<uint32_t>(sizeof(Key));
        }
        inline KeyHash GetHash() const {
            return KeyHash{ Utility::GetHashCode(key_) };
        }

        /// Comparison operators.
        inline bool operator==(const Key& other) const {
            return key_ == other.key_;
        }
        inline bool operator!=(const Key& other) const {
            return key_ != other.key_;
        }

    private:
        uint64_t key_;
    };

    class GenLock {
    public:
        GenLock()
                : control_{ 0 } {
        }
        GenLock(uint64_t control)
                : control_{ control } {
        }
        inline GenLock& operator=(const GenLock& other) {
            control_ = other.control_;
            return *this;
        }

        union {
            struct {
                uint64_t gen_number : 62;
                uint64_t locked : 1;
                uint64_t replaced : 1;
            };
            uint64_t control_;
        };
    };
    static_assert(sizeof(GenLock) == 8, "sizeof(GenLock) != 8");

    class AtomicGenLock {
    public:
        AtomicGenLock()
                : control_{ 0 } {
        }
        AtomicGenLock(uint64_t control)
                : control_{ control } {
        }

        inline GenLock load() const {
            return GenLock{ control_.load() };
        }
        inline void store(GenLock desired) {
            control_.store(desired.control_);
        }

        inline bool try_lock(bool& replaced) {
            replaced = false;
            GenLock expected{ control_.load() };
            expected.locked = 0;
            expected.replaced = 0;
            GenLock desired{ expected.control_ };
            desired.locked = 1;

            if(control_.compare_exchange_strong(expected.control_, desired.control_)) {
                return true;
            }
            if(expected.replaced) {
                replaced = true;
            }
            return false;
        }
        inline void unlock(bool replaced) {
            if(!replaced) {
                // Just turn off "locked" bit and increase gen number.
                uint64_t sub_delta = ((uint64_t)1 << 62) - 1;
                control_.fetch_sub(sub_delta);
            } else {
                // Turn off "locked" bit, turn on "replaced" bit, and increase gen number
                uint64_t add_delta = ((uint64_t)1 << 63) - ((uint64_t)1 << 62) + 1;
                control_.fetch_add(add_delta);
            }
        }

    private:
        std::atomic<uint64_t> control_;
    };
    static_assert(sizeof(AtomicGenLock) == 8, "sizeof(AtomicGenLock) != 8");

    class Value {
    public:
        Value()
                : gen_lock_{ 0 }
                , size_{ 0 }
                , length_{ 0 } {
        }

        inline uint32_t size() const {
            return size_;
        }

        friend class RmwContext;
        friend class ReadContext;
        friend class UpsertContext;

    private:
        AtomicGenLock gen_lock_;
        uint32_t size_;
        uint32_t length_;

        inline const int8_t* buffer() const {
            return reinterpret_cast<const int8_t*>(this + 1);
        }
        inline int8_t* buffer() {
            return reinterpret_cast<int8_t*>(this + 1);
        }
    };

    using store_t = FasterKv<Key, Value, disk_t>;

    size_t hash_size=1048576;    // hash table size (number of 64-byte buckets)
    size_t log_size=256*1024*1024;    //log_size == 256MB
    store_t store{hash_size, log_size, "tmp", 0.5};    // hash_table_size, log_size, filename, log_mutable_fraction

    class UpsertContext : public IAsyncContext {
    public:
        typedef Key key_t;
        typedef Value value_t;

        UpsertContext(uint64_t key, uint32_t length)
                : key_{ key }
                , length_{ length } {
        }

        /// Copy (and deep-copy) constructor.
        UpsertContext(const UpsertContext& other)
                : key_{ other.key_ }
                , length_{ other.length_ } {
        }

        /// The implicit and explicit interfaces require a key() accessor.
        inline const Key& key() const {
            return key_;
        }
        inline uint32_t value_size() const {
            //std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << "upsert " <<  sizeof(Value) + length_ << std::endl;
            return sizeof(Value) + length_;
        }
        /// Non-atomic and atomic Put() methods.
        inline void Put(Value& value) {

            value.length_ = length_;
            // //std::cout << "value length- upsert" << value.length_ << std::endl;
            std::memset(value.buffer(), 'v', length_);
        }
        inline bool PutAtomic(Value& value) {
            if(value.length_ < sizeof(Value) + length_) {
                // Current value is too small for in-place update.
                return false;
            }
            // In-place update overwrites length and buffer, but not size.
            value.length_ = length_;
            std::memset(value.buffer(), 'u', length_);
            return true;
        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }

    private:
        Key key_;
        uint32_t length_;
    };

    class ReadContext : public IAsyncContext {
    public:
        typedef Key key_t;
        typedef Value value_t;

        ReadContext(uint64_t key)
                : key_{ key }
                , output_length{ 0 } {
        }

        /// Copy (and deep-copy) constructor.
        ReadContext(const ReadContext& other)
                : key_{ other.key_ }
                , output_length{ 0 } {
        }

        /// The implicit and explicit interfaces require a key() accessor.
        inline const Key& key() const {
            return key_;
        }

        inline void Get(const Value& value) {
            // All reads should be atomic (from the mutable tail).
            //ASSERT_TRUE(false);
        }
        inline void GetAtomic(const Value& value) {
            GenLock before, after;
            // fixme - we only read two letter - Fast gets for Faster
            do {
                before = value.gen_lock_.load();
                ////std::cout << "in read context" << std::endl;
                output_length = value.length_;
                // //std::cout << "value.length_" <<   output_length<< std::endl;
                output_bytes[0] = value.buffer()[0];
                output_bytes[1] = value.buffer()[value.length_ - 1];
                val = value.buffer(); // new version
                //std::memcpy(value.buffer(), val, output_length ); // new version
                after = value.gen_lock_.load();
            } while(before.gen_number != after.gen_number);
        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }

    private:
        Key key_;
    public:
        uint32_t output_length; // fixme I cahnge for int8 to int32
        // Extract two bytes of output.
        const int8_t *val;
        int8_t output_bytes[2];
    };

    class DeleteContext : public IAsyncContext {
    private:
        Key key_;
    public:
        typedef Key key_t;
        typedef Value value_t;

        explicit DeleteContext(const Key& key)
                : key_{ key }
        {}

        inline const Key& key() const {
            return key_;
        }

        inline static constexpr uint32_t value_size() { //TODO(fixme)
            //return Value::size();
            //return 10; // Shown - Fixme
            ////std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << "delete " <<  sizeof(Value)<< std::endl;
            return sizeof(Value);
        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }
    };

    class RmwContext : public IAsyncContext {
    public:
        typedef Key key_t;
        typedef Value value_t;

        RmwContext(uint64_t key, int8_t incr, uint32_t length)
                : key_{ key }
                , incr_{ incr }
                , length_{ length } {
        }

        /// Copy (and deep-copy) constructor.
        RmwContext(const RmwContext& other)
                : key_{ other.key_ }
                , incr_{ other.incr_ }
                , length_{ other.length_ } {
        }

        /// The implicit and explicit interfaces require a key() accessor.
        inline const Key& key() const {
            return key_;
        }
        inline uint32_t value_size() const {
            return sizeof(value_t) + length_;
        }
        inline uint32_t value_size(const Value& old_value) const {
            //std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << "rmw " << sizeof(value_t) + length_ << std::endl;
            return sizeof(value_t) + length_;
        }

        inline void RmwInitial(Value& value) {
            ////std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << "in rmw rmw intial" << std::endl;
            value.gen_lock_.store(GenLock{});
            value.size_ = sizeof(Value) + length_;
            value.length_ = length_;
            std::memset(value.buffer(), incr_, length_);
        }
        inline void RmwCopy(const Value& old_value, Value& value) {
            // //std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << "in rmw rmw copy" << std::endl;
            value.gen_lock_.store(GenLock{});
            value.size_ = sizeof(Value) + old_value.length_ +  length_;
            value.length_ =  old_value.length_ + length_;
            std::memset(value.buffer(), incr_, old_value.length_ + length_);
            for(uint32_t idx = 0; idx < std::min(old_value.length_, old_value.length_ +  length_); ++idx) {
                //value.buffer()[idx] = old_value.buffer()[idx] + incr_; // fixme I chnaged this
                value.buffer()[idx] = old_value.buffer()[idx];
            }
        }
        inline bool RmwAtomic(Value& value) {
            return false;
            ////std::cout << "#################################" << "in rmw atomic" << std::endl;
            bool replaced;
            while(!value.gen_lock_.try_lock(replaced) && !replaced) {
                std::this_thread::yield();
            }
            if(replaced) {
                // Some other thread replaced this record.
                return false;
            }
            ////std::cout << "*****************************" << "in rmw atomic" << std::endl;
            ////std::cout << "value.size_: " << value.size_ << "sizeof(Value) + length_: " << sizeof(Value) + length_ << std::endl;
            if(value.size_ < sizeof(Value) + length_) {

                // Current value is too small for in-place update.
                value.gen_lock_.unlock(true);
                return false;
            }
            ////std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << "in rmw atomic" << std::endl;
            // In-place update overwrites length and buffer, but not size.
            value.length_ = length_;
            for(uint32_t idx = 0; idx < length_; ++idx) {
                //value.buffer()[idx] += incr_;
                value.buffer()[idx] = incr_; // fixme I changed this
            }
            value.gen_lock_.unlock(false);
            return true;
        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }

    private:
        int8_t incr_;
        uint32_t length_;
        Key key_;
    };
};



#endif //REPLAYER_PLG_FASTER_H
