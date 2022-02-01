//
// Created by Showan Asyabi on 4/21/21.
//

#ifndef GADGET_FASTER_H
#define GADGET_FASTER_H


#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <assert.h>
#include <string.h>
#include <chrono>
#include <atomic>
#include <cinttypes>
#include <numeric>
#include <vector>
#include <algorithm>
#include <filesystem>

#include <sys/types.h>
#include <sys/stat.h>

#include <experimental/filesystem>
#include "core/auto_ptr.h"
#include "core/faster.h"




typedef FASTER::environment::QueueIoHandler handler_t;
/// Disk's log uses 64 MB segments.
typedef FASTER::device::FileSystemDisk<handler_t, 67108864> disk_t;    //32MB segments

//typedef FASTER::device::FileSystemDisk<handler_t, 268435456L> disk_t;
using namespace FASTER::core;

class FasterWrapper: public KVWrapper {


    class ReadContext;
    class UpsertContext;
    class RmwContext;

    Guid store_guid;
    Guid store_guid2; // we use this for throughput experiment


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
/*
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
            return sizeof(value_t) + length_;
        }

        inline void RmwInitial(Value& value) {
            std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << "in rmw rmw intial" << std::endl;
            value.gen_lock_.store(GenLock{});
            value.size_ = sizeof(Value) + length_;
            value.length_ = length_;
            std::memset(value.buffer(), incr_, length_);
        }
        inline void RmwCopy(const Value& old_value, Value& value) {
            std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << "in rmw rmw copy" << std::endl;
            value.gen_lock_.store(GenLock{});
            value.size_ = sizeof(Value) + length_;
            value.length_ = length_;
            std::memset(value.buffer(), incr_, length_);
            for(uint32_t idx = 0; idx < std::min(old_value.length_, length_); ++idx) {
                //value.buffer()[idx] = old_value.buffer()[idx] + incr_; // fixme I chnaged this
                value.buffer()[idx] = old_value.buffer()[idx];
            }
        }
        inline bool RmwAtomic(Value& value) {
            std::cout << "#################################" << "in rmw atomic" << std::endl;
            bool replaced;
            while(!value.gen_lock_.try_lock(replaced) && !replaced) {
                std::this_thread::yield();
            }
            if(replaced) {
                // Some other thread replaced this record.
                return false;
            }
            std::cout << "*****************************" << "in rmw atomic" << std::endl;
            std::cout << "value.size_: " << value.size_ << "sizeof(Value) + length_: " << sizeof(Value) + length_ << std::endl;
            if(value.size_ < sizeof(Value) + length_) {

                // Current value is too small for in-place update.
                value.gen_lock_.unlock(true);
                return false;
            }
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << "in rmw atomic" << std::endl;
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
*/



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




    using store_t = FasterKv<Key, Value, disk_t>;

    size_t hash_size=1048576;    // hash table size (number of 64-byte buckets) == 64MB
    size_t log_size=256*1024*1024;;    //log_size == 256m
    store_t store{hash_size, log_size, "tmp", 0.5};    // hash_table_size, log_size, filename

    store_t store2{hash_size, log_size, "tmp2", 0.5};    //  we make one for throughput experiment





public:
    explicit FasterWrapper(std::string path) {
        //connectionString_ = connectionString;
        //TODO fixme
        kDBPath = path;
        responseTime = 0;
    }

    bool connect() override {
        // make needed folders
        std::filesystem::remove_all("tmp");
        std::filesystem::remove_all("tmp2");
        if (!std::filesystem::is_directory("tmp") || !std::filesystem::exists("tmp")) {
            std::filesystem::create_directory("tmp"); // create src folder
        }
        if (!std::filesystem::is_directory("tmp2") || !std::filesystem::exists("tmp2")) {
            std::filesystem::create_directory("tmp2"); // create src folder
        }


        store_guid = store.StartSession();
        store_guid2 = store2.StartSession();
        std::cout << "Connected to Faster successfully" << std::endl;
        //uintTest(); //Turn this line on for some tests
        return true;
    }

    bool disconnect() override {
        auto callback = [](Status result, uint64_t persistent_serial_num) {
            if(result != Status::Ok) {
                printf("FASTER checkpoint failed\n");
            } else {
                printf("FASTER checkpoint successed\n");
            }
        };
        Guid token;
        //bool success = store.Checkpoint(nullptr, callback, token);

        store.CompletePending(true);
        store.StopSession();
        store2.CompletePending(true);
        store2.StopSession();


        return(0);
    }


    void runBatchLatency()  override{
        /*
        uint64_t idx = 0;
        responseTimes.resize(operationsList.size());
        //fasterKeys.resize(keysList.size());
        //std::unordered_map<std::string, uint64_t> hashMap;
        //uint64_t  keyindex =0;
        valueSize = 256;
        std::cout << "valuesList[0].size(); " << valueSize << std::endl;
        for(int i =0; i < keysList.size(); i++ ) {
            if(hashMap.find(keysList[i]) !=  hashMap.end()) {
                fasterKeys[i] = hashMap[keysList[i]];
            } else {
                keyindex ++;
                hashMap[keysList[i]] = keyindex;
                fasterKeys[i] =keyindex;
            }
        }

        assert(valuesList.size() == keysList.size());
        assert(fasterKeys.size() == keysList.size());
        assert(operationsList.size() == keysList.size());
        //std::cout << "the key size" << keysList[idx].size();
        //std::cout << "the value size 2: " << valuesList[0] << std::endl;
        uint64_t loopSize = keysList.size();
        std::cout << "I am here" << std::endl;
        while (idx < loopSize ) {
            std::cout << "opr " << operationsList[idx] <<  " key: " <<fasterKeys[idx]  << "value " << valuesList[idx] << "valueSize: "  << valueSize << std::endl;
            switch (operationsList[idx]) {

                case Operation::putOpr: {
                    startTime = std::chrono::high_resolution_clock::now();
                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        CallbackContext<UpsertContext> context{ ctxt };
                    };
                    UpsertContext context{ fasterKeys[idx], valueSize};
                    Status result = store.Upsert(context, callback, 1);
                    if(result==Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                        bool _pendingres = store.CompletePending(true);
                        if (_pendingres)
                            result = Status::Ok;
                    }
                    //std::cout << "ok "<< (Status::Ok==result) << " put " <<   " val: " <<  11 << " key: " << fasterKeys[idx] << " my key: "  << keysList[idx] <<  std::endl;
                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(Status::Ok==result);
                    break;
                }
                case Operation::getOpr: {
                    startTime = std::chrono::high_resolution_clock::now();
                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        CallbackContext<ReadContext> context{ ctxt };
                    };
                    ReadContext context{fasterKeys[idx]};
                    Status result = store.Read(context, callback, 1);
                    if(result==Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                        bool _pendingres = store.CompletePending(true);
                        if (_pendingres)
                            result = Status::Ok;
                    }
//
//                    if(Status::Ok==result) {
//                        std::cout << "ok " << (Status::Ok == result) << " get " << " gotten val: "
//                                  //<< context.output_pt1[valueSize - 1] << " key: " << fasterKeys[idx] << " my key"
//                                << context.output_letters[0] << " key: " << fasterKeys[idx] << " my key"
//                                  << keysList[idx] << std::endl;
//                    } else {
//                        std::cout << "ok "<< (Status::Ok==result) << " get "   << " key: " << fasterKeys[idx] << " my key"  << keysList[idx] <<  std::endl;
//                    }
//                    if(result == Status::Ok)
//                    std::cout<<  "result" << "ok" <<std::endl;
//                    if(result == Status::NotFound)
//                        std::cout<<  "result" << "not found" <<std::endl;
//                    if(result == Status::IOError)
//                        std::cout<<  "result" << "Io  error" <<std::endl;
//
//                    assert(Status::Ok==result);
                    finishTime = std::chrono::high_resolution_clock::now();
                    break;
                }
                case Operation::mergeOpr: {
                    startTime = std::chrono::high_resolution_clock::now();
                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        //In-memory test.
                        // ASSERT_TRUE(false);
                    };
                    RmwContext context{fasterKeys[idx] , 1, valueSize };
                    Status result = store.Rmw(context, callback, 1);
                    if(result==Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                        bool _pendingres = store.CompletePending(true);
                        if (_pendingres)
                            result = Status::Ok;
                    }
                    //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << fasterKeys[idx] << " my key"  << keysList[idx] <<  std::endl;
                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(Status::Ok==result);
                    break;
                }

                case Operation::deleteOpr: {
                    startTime = std::chrono::high_resolution_clock::now();
                    auto callback = [](IAsyncContext *ctxt, Status result) {
                        CallbackContext<DeleteContext> context{ctxt};
                    };

                    DeleteContext context{fasterKeys[idx]};
                    Status result = store.Delete(context, callback, 1);
                    //std::cout << "ok "<< (Status::Ok==result) << " delete " <<   " val: " <<  valuesList[idx] << "key: " << fasterKeys[idx] << " my key: "  << keysList[idx] <<  std::endl;
                    //assert(Status::Ok == result);
                    if(result==Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                        bool _pendingres = store.CompletePending(true);
                        if (_pendingres)
                            result = Status::Ok;
                    }
                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(Status::Ok==result);
                    break;

                }
            }

            auto commandExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
            responseTimes[idx] = commandExecutionTime;
            idx ++;
            std::cout << "idx: l: " << idx << std::endl;
        }
        */
    }

    double runBatchThroughput() override{

        uint64_t idx = 0;
        responseTimes.resize(operationsList.size());
        fasterKeys.resize(keysList.size());
        std::unordered_map<std::string, uint64_t> hashMap;
        uint64_t  keyindex =0;
        valueSize = valuesList[0].size();
        /*int trash = 0;
        for(int i =0; i < keysList.size(); i++ ) {
            if ("00000000000000000000000000000000000000000000000000000000000011562" == keysList[i] ) {
                trash ++;
            }
        }*/
        //std::cout << "trash" << trash << std::endl;
        //uint64_t  trah_key;
        for(int i =0; i < keysList.size(); i++ ) {
            if(hashMap.find(keysList[i]) !=  hashMap.end()) {
                fasterKeys[i] = hashMap[keysList[i]];
            } else {
                keyindex ++;
                hashMap[keysList[i]] = keyindex;
                fasterKeys[i] =keyindex;
            }
        }




        assert(valuesList.size() == keysList.size());
        assert(fasterKeys.size() == keysList.size());
        assert(operationsList.size() == keysList.size());
        assert(valuesList.size() > 0);
        assert(valuesList.size() == keysList.size());
        assert(valuesList.size() == operationsList.size());
        uint64_t loopSize = keysList.size();
        startTime = std::chrono::high_resolution_clock::now();
        while (idx < loopSize) {
            //std::cout << "opr " << operationsList[idx] <<  " key: " <<fasterKeys[idx]  << "valueSize: "  << valueSize << std::endl;
            switch (operationsList[idx]) {
                case Operation::putOpr:
                {
                    oprStartTime = std::chrono::high_resolution_clock::now();
                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        CallbackContext<UpsertContext> context{ ctxt };
                    };
                    UpsertContext context{ fasterKeys[idx], valueSize};
                    Status result = store2.Upsert(context, callback, 1);
                    if(result==Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                        bool _pendingres = store2.CompletePending(true);
                        if (_pendingres)
                            result = Status::Ok;
                    }
                    //std::cout << "ok "<< (Status::Ok==result) << " put " <<   " val: " <<  11 << " key: " << fasterKeys[idx] << " my key: "  << keysList[idx] <<  std::endl;
                    oprFinishTime = std::chrono::high_resolution_clock::now();
                    break;

                }
                case Operation::getOpr:
                {
                    oprStartTime = std::chrono::high_resolution_clock::now();
                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        CallbackContext<ReadContext> context{ ctxt };
                    };
                    ReadContext context{fasterKeys[idx]};
                    Status result = store2.Read(context, callback, 1);
                    if(result==Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                        bool _pendingres = store2.CompletePending(true);
                        if (_pendingres)
                            result = Status::Ok;
                    }

                    // new version
                    if(result == Status::Ok) {
                        uint32_t oldValueSize = context.output_length;
                        char buffer[oldValueSize];
                        std::memcpy(buffer, context.val, oldValueSize);
                    }
                    //std::cout<<  "result" << "o k" <<std::endl;
                    //if(result == Status::NotFound)
                    //   std::cout<<  "result" << "not found" <<std::endl;
                    //if(result == Status::IOError)
                    //   std::cout<<  "result" << "Io  error" <<std::endl;

                    //assert(Status::Ok==result);
                    oprFinishTime = std::chrono::high_resolution_clock::now();
                    break;
                }
                case Operation::mergeOpr:
                {
                    oprStartTime = std::chrono::high_resolution_clock::now();
                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        CallbackContext<RmwContext> context{ctxt};
                        // ASSERT_TRUE(false);
                    };
                    RmwContext context{fasterKeys[idx] , 1, valueSize };
                    Status result = store2.Rmw(context, callback, 1);
                    if(result==Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                        bool _pendingres = store2.CompletePending(true);
                        if (_pendingres)
                            result = Status::Ok;
                    }
                    //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << fasterKeys[idx] << " my key"  << keysList[idx] <<  std::endl;




//                    //get
//                    auto callback = [](IAsyncContext *ctxt, Status result) {
//                        CallbackContext <ReadContext> context{ctxt};
//                    };
//                    ReadContext context{fasterKeys[idx]};
//                    Status result = store2.Read(context, callback, 1);
//                    if (result ==Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
//                        bool _pendingres = store2.CompletePending(true);
//                        if (_pendingres)
//                            result = Status::Ok;
//                    }
//
//                    // if this is the first merge
//                    if (Status::Ok != result) {
//                        // just put the value
//                        auto callback = [](IAsyncContext *ctxt, Status result) {
//                            CallbackContext <UpsertContext> context{ctxt};
//                        };
//                        UpsertContext context{ fasterKeys[idx], valueSize};
//                        Status result = store2.Upsert(context, callback, 1);
//
//                        assert(Status::Ok == result);
//
//                    } else { // this is not the first merge on this key
//                        // append to the value
//                        uint32_t oldValueSize = context.output_length;
//                        //std::cout << "old value size" << oldValueSize << std::endl;
//
//
//                        char buffer[oldValueSize + valueSize];
//                        std::memcpy(buffer, context.val, oldValueSize);
//
//                        //append to it
//                        // and upsert it
//                        //std::cout << "I am here" << std::endl;
//
//                        auto callback = [](IAsyncContext *ctxt, Status result) {
//                            CallbackContext <UpsertContext> context{ctxt};
//                        };
//                        UpsertContext context{fasterKeys[idx], oldValueSize + valueSize};
//                        Status result = store2.Upsert(context, callback, 1);
//                        //std::cout << "I am here 3" << std::endl;
//
//                        assert(Status::Ok == result);
//                    }
                    oprFinishTime = std::chrono::high_resolution_clock::now();
                    break;
                }

                case Operation::deleteOpr: {
                    oprStartTime = std::chrono::high_resolution_clock::now();
                    //Key key{ std::make_pair(idx % 7, thread_idx * kNumOps + idx) };
                    auto callback = [](IAsyncContext *ctxt, Status result) {
                        CallbackContext<DeleteContext> context{ctxt};
                    };

                    DeleteContext context{fasterKeys[idx]};
                    Status result = store2.Delete(context, callback, 1);
                    if(result==Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                        bool _pendingres = store2.CompletePending(true);
                        if (_pendingres)
                            result = Status::Ok;
                    }
                    oprFinishTime = std::chrono::high_resolution_clock::now();
                    break;
                }
            }

            auto commandExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( oprFinishTime - oprStartTime ).count();
            responseTimes[idx] = commandExecutionTime;
            idx ++;

           //if(idx % 10000 == 0) {
               //std::cout << "idx: " << idx << std::endl;
               //std::cout << valuesList.size() << "-0-" << valuesList[0].size() << "-1- " <<valuesList[1].size()  << "-2- " <<valuesList[2].size() << " last " <<valuesList[valuesList.size()-1].size()   << std::endl;
               //std::cout << "operation" << operationsList.size() << "-0-" << operationsList[0]<< "-1- " <<operationsList[1]  << "-2- " <<operationsList[2] << "n " <<operationsList[operationsList.size() -1 ]  << std::endl;
               //std::cout << "keys" << fasterKeys.size() << "-0-" << fasterKeys[0] << "-1- " <<fasterKeys[1]  << "-2- " <<fasterKeys[2] << "n " <<fasterKeys[fasterKeys.size() -1]<< std::endl;
           //}
        }
        finishTime = std::chrono::high_resolution_clock::now();
        auto ExperimentExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();

       // std::cout << "ExperimentExecutionTime: " << ExperimentExecutionTime << " operationsList.size:  " << operationsList.size() << std::endl;
       // std::cout << "idx: " << idx << std::endl;
       //std::cout << valuesList.size() << valuesList[0].size() << std::endl;
        // fixme - should we add complete pending here  e.g.,  store2.CompletePending(true);
        //std::cout << "Throughput " << (double)operationsList.size()/ (double)ExperimentExecutionTime * 1000000 << std::endl;
        return (double)operationsList.size()/ (double)ExperimentExecutionTime; // Throughput
    }
    /*
    bool command(Operation::OperationType opr, const std::string& key,  std::string_view value) override {
        assert(key.size() > 0);
        assert(value.length() > 0);
        switch (opr) {
            case Operation::putOpr:
                startTime = std::chrono::high_resolution_clock::now();
                db->Put(rocksdb::WriteOptions(), key, rocksdb::Slice(value.data(), value.size()));
                finishTime = std::chrono::high_resolution_clock::now();
                break;
            case Operation::getOpr:
                startTime = std::chrono::high_resolution_clock::now();
                db->Get(rocksdb::ReadOptions(), key  , &response);
                finishTime = std::chrono::high_resolution_clock::now();
                break;
            case Operation::mergeOpr:
                 startTime = std::chrono::high_resolution_clock::now();
                 db->Merge(rocksdb::WriteOptions(), key, rocksdb::Slice(value.data(), value.size()));
                 finishTime = std::chrono::high_resolution_clock::now();
                break;
            case Operation::deleteOpr:
                 startTime = std::chrono::high_resolution_clock::now();
                 db->Delete(rocksdb::WriteOptions(), rocksdb::Slice(value.data(), value.size()));
                 finishTime = std::chrono::high_resolution_clock::now();
                 break;
        }
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
        responseTime = duration;
        //std::cout<<duration<<std::endl;
        // TODO remove me
        return  s.ok();
    }

   */

    void uintTest() {
        // get

        valueSize = 256;

        std::cout<< "I am here" << std::endl;

        {
            auto callback = [](IAsyncContext *ctxt, Status result) {
                CallbackContext <ReadContext> context{ctxt};
            };
            ReadContext context{1};
            Status result = store.Read(context, callback, 1);
            if (result == Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                bool _pendingres = store.CompletePending(true);
                if (_pendingres)
                    result = Status::Ok;
            }
            //  expect:
            assert(Status::Ok!=result);

        }
        std::cout<< "I am here2" << std::endl;


        // put
        {
            auto callback = [](IAsyncContext* ctxt, Status result) {
                CallbackContext<UpsertContext> context{ ctxt };
            };
            UpsertContext context{ 1, valueSize};
            Status result = store.Upsert(context, callback, 1);

            assert(Status::Ok==result);
        }

        std::cout<< "I am here3" << std::endl;
        // get
        { auto callback = [](IAsyncContext *ctxt, Status result) {
                CallbackContext <ReadContext> context{ctxt};
            };
            ReadContext context{1};
            Status result = store.Read(context, callback, 1);
            if (result == Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                bool _pendingres = store.CompletePending(true);
                if (_pendingres)
                    result = Status::Ok;
            }

            assert(Status::Ok==result);
            std::cout<< "context.output_length" << context.output_length << std::endl;
            assert(context.output_length == valueSize);
            assert(context.output_bytes[0] == 'v'); // first letter
            assert(context.output_bytes[1] == 'v'); // last letter


        }
        std::cout<< "I am here4" << std::endl;
        // put
        {
            auto callback = [](IAsyncContext* ctxt, Status result) {
                CallbackContext<UpsertContext> context{ ctxt };
            };
            UpsertContext context{ 1, valueSize};
            Status result = store.Upsert(context, callback, 1);

            assert(Status::Ok==result);

        }
        // delete
        {
            auto callback = [](IAsyncContext *ctxt, Status result) {
                CallbackContext<DeleteContext> context{ctxt};
            };

            DeleteContext context{1};
            Status result = store.Delete(context, callback, 1);
            assert(Status::Ok==result);
        }
        std::cout<< "I am here5" << std::endl;
        //get
        {
            auto callback = [](IAsyncContext *ctxt, Status result) {
                CallbackContext <ReadContext> context{ctxt};
            };
            ReadContext context{1};
            Status result = store.Read(context, callback, 1);
            if (result == Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                bool _pendingres = store.CompletePending(true);
                if (_pendingres)
                    result = Status::Ok;
            }
            assert(Status::Ok!=result);


        }


        // rmw tests
        // increase the length from 10 to 10000 and check the size
        /*
        for (int size = 10; size < 100000; size+= 10) {

            // rmw
            {
                auto callback = [](IAsyncContext *ctxt, Status result) {
                    //In-memory test.
                    // ASSERT_TRUE(false);
                };
                RmwContext context{1, 1, size};
                Status result = store.Rmw(context, callback, 1);
                //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << 1 << " my key"  << keysList[idx] <<  std::endl;
                assert(Status::Ok == result);
            }
            //get
            {
                auto callback = [](IAsyncContext *ctxt, Status result) {
                    CallbackContext <ReadContext> context{ctxt};
                };
                ReadContext context{1};
                Status result = store.Read(context, callback, 1);
                if (result ==
                    Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                    bool _pendingres = store.CompletePending(true);
                    if (_pendingres)
                        result = Status::Ok;
                }
                assert(Status::Ok == result);
                std::cout << "context.output_length" << context.output_length << std::endl;
                assert(context.output_length == size);
                //assert(context.output_bytes[0] == 1);
                //assert(context.output_bytes[1] == 1);
            }
        }

        for (int size = 10; size <= 100; size+= 10) {

            // rmw
            {
                auto callback = [](IAsyncContext *ctxt, Status result) {
                    //In-memory test.
                    // ASSERT_TRUE(false);
                };
                RmwContext context{2, (size/10) % 10 , size};
                Status result = store.Rmw(context, callback, 1);
                //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << 1 << " my key"  << keysList[idx] <<  std::endl;
                assert(Status::Ok == result);
            }
            //get
            {
                auto callback = [](IAsyncContext *ctxt, Status result) {
                    CallbackContext <ReadContext> context{ctxt};
                };
                ReadContext context{2};
                Status result = store.Read(context, callback, 1);
                if (result ==
                    Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                    bool _pendingres = store.CompletePending(true);
                    if (_pendingres)
                        result = Status::Ok;
                }
                assert(Status::Ok == result);
                std::cout << "context.output_length" << context.output_length << std::endl;
                std::cout<< "first element " <<  unsigned(context.output_bytes[0]) << std::endl;
                std::cout<< "last element " <<  unsigned(context.output_bytes[1]) << std::endl;
                assert(context.output_length == size);
                assert(context.output_bytes[0] == 1);
                assert(context.output_bytes[1] == (size/10) % 10);

            }
        }

        */

   // get append put
        {
            for (int size = 10; size < 100000; size += 10) {


                //get
                auto callback = [](IAsyncContext *ctxt, Status result) {
                    CallbackContext <ReadContext> context{ctxt};
                };
                ReadContext context{12};
                Status result = store.Read(context, callback, 1);
                if (result ==Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                    bool _pendingres = store.CompletePending(true);
                    if (_pendingres)
                        result = Status::Ok;
                }

                // if this is the first merge
                if (Status::Ok != result) {
                    // just put the value
                    auto callback = [](IAsyncContext *ctxt, Status result) {
                        CallbackContext <UpsertContext> context{ctxt};
                    };
                    UpsertContext context{12, valueSize};
                    Status result = store.Upsert(context, callback, 1);

                    assert(Status::Ok == result);

                } else { // this is not the first merge on this key


                    // append to the value
                    uint32_t oldValueSize = context.output_length;
                    std::cout << "old value size" << oldValueSize << std::endl;

                    char buffer[oldValueSize + valueSize];
                    std::memcpy(buffer, context.val, oldValueSize);

                    //append to it
                    // and upsert it

                    auto callback = [](IAsyncContext *ctxt, Status result) {
                        CallbackContext <UpsertContext> context{ctxt};
                    };
                    UpsertContext context{12, oldValueSize + valueSize};
                    Status result = store.Upsert(context, callback, 1);

                    assert(Status::Ok == result);





                }

            }
        }



        for (int size = 10; size < 100000; size+= 10) {
            std::cout << "size_: " << size << std::endl;

            // rmw
            {
                auto callback = [](IAsyncContext *ctxt, Status result) {
                    //In-memory test.
                    // ASSERT_TRUE(false);
                };
                RmwContext context{3, 1, 10};
                Status result = store.Rmw(context, callback, 1);
                //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << 1 << " my key"  << keysList[idx] <<  std::endl;
                assert(Status::Ok == result);
            }
            //get
            {
                auto callback = [](IAsyncContext *ctxt, Status result) {
                    CallbackContext <ReadContext> context{ctxt};
                };
                ReadContext context{3};
                Status result = store.Read(context, callback, 1);
                if (result ==
                    Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                    bool _pendingres = store.CompletePending(true);
                    if (_pendingres)
                        result = Status::Ok;
                }
                assert(Status::Ok == result);
                std::cout << "context.output_length: " << context.output_length << std::endl;
                std::cout << "size: " << size << std::endl;
                assert(context.output_length == size);
                //assert(context.output_bytes[0] == 1);
                //assert(context.output_bytes[1] == 1);
            }
        }

        for (int size = 10; size <= 100; size+= 10) {

            // rmw
            {
                auto callback = [](IAsyncContext *ctxt, Status result) {
                    //In-memory test.
                    // ASSERT_TRUE(false);
                };
                RmwContext context{2, (size/10) % 10 , 10};
                Status result = store.Rmw(context, callback, 1);
                //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << 1 << " my key"  << keysList[idx] <<  std::endl;
                assert(Status::Ok == result);
            }
            //get
            {
                auto callback = [](IAsyncContext *ctxt, Status result) {
                    CallbackContext <ReadContext> context{ctxt};
                };
                ReadContext context{2};
                Status result = store.Read(context, callback, 1);
                if (result ==
                    Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                    bool _pendingres = store.CompletePending(true);
                    if (_pendingres)
                        result = Status::Ok;
                }
                assert(Status::Ok == result);
                std::cout << "context.output_length" << context.output_length << std::endl;
                std::cout<< "first element " <<  unsigned(context.output_bytes[0]) << std::endl;
                std::cout<< "last element " <<  unsigned(context.output_bytes[1]) << std::endl;
                assert(context.output_length == size);
                assert(context.output_bytes[0] == 1);
                assert(context.output_bytes[1] == (size/10) % 10);

            }
        }

        std::cout<< "I am here6" << std::endl;
        // rmw
        {
            auto callback = [](IAsyncContext* ctxt, Status result) {
                //In-memory test.
                // ASSERT_TRUE(false);
            };
            RmwContext context{1 , 1 , 100};
            Status result = store.Rmw(context, callback, 1);
            //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << 1 << " my key"  << keysList[idx] <<  std::endl;
            assert(Status::Ok==result);

        }
        std::cout<< "I am here7" << std::endl;
        //get
        {
            auto callback = [](IAsyncContext *ctxt, Status result) {
                CallbackContext <ReadContext> context{ctxt};
            };
            ReadContext context{1};

            Status result = store.Read(context, callback, 1);
            if (result == Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                bool _pendingres = store.CompletePending(true);
                if (_pendingres)
                    result = Status::Ok;
            }
            std::cout<< "context.output_length" << context.output_length << std::endl;
            assert(Status::Ok==result);
            assert(context.output_length == 100);
            assert(context.output_bytes[0] == 1);
            assert(context.output_bytes[1] == 1);
        }
        std::cout<< "I am here8" << std::endl;
        // rmw
        { auto callback = [](IAsyncContext* ctxt, Status result) {
                //In-memory test.
                // ASSERT_TRUE(false);
            };
            RmwContext context{1 , 5 , 16};
            Status result = store.Rmw(context, callback, 1);
            //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << 1 << " my key"  << keysList[idx] <<  std::endl;
            assert(Status::Ok==result);
        }
        std::cout<< "I am here9" << std::endl;
        // get
        {
            auto callback = [](IAsyncContext *ctxt, Status result) {
                CallbackContext <ReadContext> context{ctxt};
            };
            ReadContext context{1};
            Status result = store.Read(context, callback, 1);
            if (result == Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                bool _pendingres = store.CompletePending(true);
                if (_pendingres)
                    result = Status::Ok;
            }
            assert(Status::Ok==result);
            assert(context.output_length == 116);
            assert(context.output_bytes[0] == 1);
            std::cout<< "context.output_length" << context.output_length << std::endl;
            //std::cout<< "last latter: " << context.output_bytes[1] << std::endl;
            assert(context.output_bytes[1] == 5);
        }
        std::cout<< "I am here 10" << std::endl;
        // rmw
        { auto callback = [](IAsyncContext* ctxt, Status result) {
                //In-memory test.
                // ASSERT_TRUE(false);
            };
            RmwContext context{1 , 1 , 5};
            Status result = store.Rmw(context, callback, 1);
            //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << 1 << " my key"  << keysList[idx] <<  std::endl;
            assert(Status::Ok==result);
        }
        std::cout<< "I am here11" << std::endl;
        // rmw
        { auto callback = [](IAsyncContext* ctxt, Status result) {
                //In-memory test.
                // ASSERT_TRUE(false);
            };
            RmwContext context{1 , 1 , 5};
            Status result = store.Rmw(context, callback, 1);
            //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << 1 << " my key"  << keysList[idx] <<  std::endl;
            assert(Status::Ok==result);
        }

        // delete
        {
            auto callback = [](IAsyncContext *ctxt, Status result) {
                CallbackContext<DeleteContext> context{ctxt};
            };

            DeleteContext context{1};
            Status result = store.Delete(context, callback, 1);
            assert(Status::Ok==result);
        }

        std::cout<< "I am here12" << std::endl;
        // rmw
        { auto callback = [](IAsyncContext* ctxt, Status result) {
                //In-memory test.
                // ASSERT_TRUE(false);
            };
            RmwContext context{1 , 2 , 5};
            Status result = store.Rmw(context, callback, 1);
            //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << 1 << " my key"  << keysList[idx] <<  std::endl;
            assert(Status::Ok==result);
        }


        // rmw
        { auto callback = [](IAsyncContext* ctxt, Status result) {
                //In-memory test.
                // ASSERT_TRUE(false);
            };
            RmwContext context{1 , 4 , 10};
            Status result = store.Rmw(context, callback, 1);
            //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << 1 << " my key"  << keysList[idx] <<  std::endl;
            assert(Status::Ok==result);
        }

        // get
        {
            auto callback = [](IAsyncContext *ctxt, Status result) {
                CallbackContext <ReadContext> context{ctxt};
            };
            ReadContext context{1};
            Status result = store.Read(context, callback, 1);
            if (result == Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                bool _pendingres = store.CompletePending(true);
                if (_pendingres)
                    result = Status::Ok;
            }
            assert(Status::Ok==result);
            std::cout<< "context.output_length" << context.output_length << std::endl;
            assert(context.output_length == 15);
            assert(context.output_bytes[0] == 2);
            //std::cout<< "last latter: " << context.output_bytes[1] << std::endl;
            assert(context.output_bytes[1] == 4);
        }
        std::cout<< "I am here13" << std::endl;

        // delete
        {
            auto callback = [](IAsyncContext *ctxt, Status result) {
                CallbackContext<DeleteContext> context{ctxt};
            };

            DeleteContext context{1};
            Status result = store.Delete(context, callback, 1);
            assert(Status::Ok==result);
        }
        // get
        {
            auto callback = [](IAsyncContext *ctxt, Status result) {
                CallbackContext <ReadContext> context{ctxt};
            };
            ReadContext context{1};
            Status result = store.Read(context, callback, 1);
            if (result == Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                bool _pendingres = store.CompletePending(true);
                if (_pendingres)
                    result = Status::Ok;
            }
            assert(Status::Ok!=result);
        }
        std::cout<< "I am here14" << std::endl;
        assert( 0 == 1);
    }

    void  dumpOperationsOnFile() {}
private:
    std::string connectionString_;
    std::string kDBPath;
    rocksdb::DB* db;
    rocksdb::Options options;
    rocksdb::Status s;
    TimeVar startTime, finishTime;
    TimeVar oprStartTime, oprFinishTime;
    std::string response;
    std::vector<uint64_t> fasterKeys;
    store_t* myStore;
    uint32_t  valueSize;
};




#endif //GADGET_FASTER_H