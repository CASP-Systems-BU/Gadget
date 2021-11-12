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


#define VALUESIZE 2048
// fixme we should get this from the value list


typedef FASTER::environment::QueueIoHandler handler_t;
typedef FASTER::device::FileSystemDisk<handler_t, 33554432L> disk_t;    //32MB segments
using namespace FASTER::core;

/*
/// Class passed to store_t::RMW().
class RmwContext : public IAsyncContext {
public:
    typedef FasterKey key_t;
    typedef FasterValue value_t;

    RmwContext(uint64_t key, uint64_t incr)
            : key_{ key }
            , incr_{ incr } {
    }

    /// Copy (and deep-copy) constructor.
    RmwContext(const RmwContext& other)
            : key_{ other.key_ }
            , incr_{ other.incr_ } {
    }

    /// The implicit and explicit interfaces require a key() accessor.
    const FasterKey& key() const {
        return key_;
    }
    inline static constexpr uint32_t value_size() {
        return sizeof(value_t);
    }
    inline static constexpr uint32_t value_size(const value_t& old_value) {
        return sizeof(value_t);
    }

    /// Initial, non-atomic, and atomic RMW methods.
    inline void RmwInitial(value_t& value) {
        value.value_ = incr_;
    }
    inline void RmwCopy(const value_t& old_value, value_t& value) {
        value.value_ = old_value.value_ + incr_;
    }
    inline bool RmwAtomic(value_t& value) {
        value.atomic_value_.fetch_add(incr_);
        return true;
    }

protected:
    /// The explicit interface requires a DeepCopy_Internal() implementation.
    Status DeepCopy_Internal(IAsyncContext*& context_copy) {
        return IAsyncContext::DeepCopy_Internal(*this, context_copy);
    }

private:
    FasterKey key_;
    uint64_t incr_;
};

*/
class FasterWrapper: public KVWrapper {


    class ReadContext;
    class UpsertContext;
    class RmwContext;

    Guid store_guid;
    Guid store_guid2; // we use this for throughput experiment



/*

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

    class alignas(16) Value {
    public:
        Value()
                : length_{ 0 }
                , value_{ 0 } {
        }

        inline static constexpr uint32_t size() {
            return static_cast<uint32_t>(sizeof(Value));
        }

        friend class UpsertContext;
        friend class ReadContext;
        friend class RmwContext;

    private:
        uint8_t value_[VALUESIZE];
        std::atomic<uint16_t> length_;
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

        inline static constexpr uint32_t value_size() {
            //return Value::size();
            return sizeof(value_t);
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
        uint32_t val_length_;
        uint8_t value_[VALUESIZE];

        RmwContext(uint64_t key, uint8_t* val, uint32_t val_length)
                : key_{ key }, val_length_(val_length) {
            //memcpy(value_, val, val_length);
        }

        /// Copy (and deep-copy) constructor.
        RmwContext(const RmwContext& other)
                : key_{ other.key_ }
                , incr_{ other.incr_ } {
        }

        /// The implicit and explicit interfaces require a key() accessor.
        inline const Key& key() const {
            return key_;
        }
        inline static constexpr uint32_t value_size() {
            return sizeof(value_t);
        }
        inline static constexpr uint32_t value_size(const Value& old_value) {
            return sizeof(value_t);
        }
        inline void RmwInitial(Value& value) {
            //value.length_ = VALUESIZE;
            //memcpy(value.value_, value_, val_length_);
            std::memset(value.value_, '$', VALUESIZE);
            //value.value = incr_;
        }
        inline void RmwCopy(const Value& old_value, Value& value) {
            //TODO: replace old value to new value. the size is the same
            //value.length_ = VALUESIZE;
            //memcpy(value.value_, value_, val_length_);
            std::memset(value.value_, '$', VALUESIZE);
            //value.value = old_value.value + incr_;
        }
        inline bool RmwAtomic(Value& value) {
            //value.atomic_value.fetch_add(incr_);
            //value.length_ = VALUESIZE;
            //memcpy(value.value_, value_, val_length_);
            std::memset(value.value_, '$', VALUESIZE);
            return true;
        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }

    private:
        int32_t incr_;    //DB[key_]=DB[key_]+incr_
        Key key_;
    };

    class UpsertContext : public IAsyncContext {
    public:
        typedef Key key_t;
        typedef Value value_t;
        uint32_t val_length_;
        uint8_t value_[VALUESIZE];

        UpsertContext(uint64_t key, uint8_t* val, uint32_t val_length)
                : key_{ key }, val_length_(val_length) {
            memcpy(value_, val, val_length);  // fixme
        }

        /// Copy (and deep-copy) constructor.
        UpsertContext(const UpsertContext& other)
                : key_{ other.key_ } {
        }

        /// The implicit and explicit interfaces require a key() accessor.
        inline const Key& key() const {
            return key_;
        }
        inline static constexpr uint32_t value_size() {
            return sizeof(value_t);
        }
        /// Non-atomic and atomic Put() methods.
        inline void Put(Value& value) {
            // add value. see L369
            value.length_ = VALUESIZE;
            memcpy(value.value_, value_, val_length_); // fixme
            std::memset(value.value_, '$', VALUESIZE);
        }
        inline bool PutAtomic(Value& value) {
            // Get the lock on the value.
            bool success;
            do {
                uint16_t expected_length;
                do {
                    // Spin until other the thread releases the lock.
                    expected_length = value.length_.load();
                } while(expected_length == UINT16_MAX);
                // Try to get the lock.
                success = value.length_.compare_exchange_weak(expected_length, UINT16_MAX);
            } while(!success);

            std::memset(value.value_, '$', VALUESIZE);
            value.length_.store(VALUESIZE);
            return true;
        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }

    private:
        Key key_;
    };

    class ReadContext : public IAsyncContext {
    public:
        typedef Key key_t;
        typedef Value value_t;

        ReadContext(uint64_t key)
                : key_{ key } {
        }

        /// Copy (and deep-copy) constructor.
        ReadContext(const ReadContext& other)
                : key_{ other.key_ } {
        }

        /// The implicit and explicit interfaces require a key() accessor.
        inline const Key& key() const {
            return key_;
        }

        inline void Get(const Value& value) {
            // All reads should be atomic (from the mutable tail).
            //ASSERT_TRUE(false);
            do {
                output_length = value.length_.load();
                //ASSERT_EQ(0, reinterpret_cast<size_t>(value.value_) % 16);
                output_pt1 = *reinterpret_cast<const uint64_t*>(value.value_);
                output_pt2 = *reinterpret_cast<const uint64_t*>(value.value_ + 8);
            } while(output_length != value.length_.load());
        }
        inline void GetAtomic(const Value& value) {
            do {
                output_length = value.length_.load();
                //ASSERT_EQ(0, reinterpret_cast<size_t>(value.value_) % 16);
                output_pt1 = *reinterpret_cast<const uint64_t*>(value.value_);
                output_pt2 = *reinterpret_cast<const uint64_t*>(value.value_ + 8);
            } while(output_length != value.length_.load());
        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }

    private:
        Key key_;
    public:
        uint16_t output_length;
        uint64_t output_pt1;
        uint64_t output_pt2;
    };

*/

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

/// This benchmark stores an 8-byte value in the key-value store.

    class Value {
    public:
        Value()
                :length_(VALUESIZE),
                 value_{ 0 }
        {
        }



        inline static constexpr uint32_t size() {
            return static_cast<uint32_t>(sizeof(Value));
        }

        friend class ReadContext;
        friend class UpsertContext;
        friend class RmwContext;

    private:
        uint8_t  value_[VALUESIZE];
        std::atomic<uint8_t> length_;
    };



    using store_t = FasterKv<Key, Value, disk_t>;

    size_t hash_size=1048576;    // hash table size (number of 64-byte buckets) == 256MB
    size_t log_size=256*1024*1024;;    //log_size == 256m
    store_t store{hash_size, log_size, "tmp", 0.5};    // hash_table_size, log_size, filename

    store_t store2{hash_size, log_size, "tmp2", 0.5};    //  we make one for throughput experiment



/// Class passed to store_t::Read().
    class ReadContext : public IAsyncContext {
    public:
        typedef Key key_t;
        typedef Value value_t;

        ReadContext(uint64_t key)
                : key_{ key } {
        }

        /// Copy (and deep-copy) constructor.
        ReadContext(const ReadContext& other)
                : key_{ other.key_ } {
        }

        /// The implicit and explicit interfaces require a key() accessor.
        inline const Key& key() const {
            return key_;
        }


        inline void Get(const Value& value) {
            //ASSERT_TRUE(false);
            //ASSERT_TRUE(false);
            output_pt1= (const uint8_t*)value.value_;

        }
        inline void GetAtomic(const Value& value) {
            do{
                output_length = value.length_.load();
                std::cout << "output_length: " << output_length << std::endl;
                //ASSERT_EQ(0, reinterpret_cast<size_t>(value.value_) % 16);
                //output_pt1 = *reinterpret_cast<const uint64_t*>(value.value_);
                output_pt1= (const uint8_t*)value.value_;
                //output_pt2 = *reinterpret_cast<const uint64_t*>(value.value_ + 8);
            } while(output_length != value.length_.load());

        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }

    private:
        Key key_;
    public:
        uint8_t output_length;
        const uint8_t *output_pt1;
        uint64_t output_pt2;
    };

/// Class passed to store_t::Upsert().
    class UpsertContext : public IAsyncContext {
    public:
        typedef Key key_t;
        typedef Value value_t;

        UpsertContext(uint64_t key, uint32_t valueLength)
                : key_{ key },
                  valueLength_{valueLength}
        { }

        /// Copy (and deep-copy) constructor.
        UpsertContext(const UpsertContext& other)
                : key_{ other.key_ }{ }

        /// The implicit and explicit interfaces require a key() accessor.
        inline const Key& key() const {
            return key_;
        }
        inline static constexpr uint32_t value_size() {
            return sizeof(value_t);
        }

        /// Non-atomic and atomic Put() methods.
        inline void Put(value_t& value) {
            value.length_ = valueLength_;
            std::memset(value.value_, 'h', VALUESIZE);
        }
        inline bool PutAtomic(value_t& value) {
            // Get the lock on the value.
            bool success;
            do {
                uint8_t expected_length;
                do {
                    // Spin until other the thread releases the lock.
                    expected_length = value.length_.load();
                } while(expected_length == UINT8_MAX);
                // Try to get the lock.
                success = value.length_.compare_exchange_weak(expected_length, UINT8_MAX);
            } while(!success);

            std::memset(value.value_, 'j', VALUESIZE);
            value.length_.store(VALUESIZE);
            return true;
        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }

    private:
        Key key_;
        uint32_t  valueLength_;
    };

/// Class passed to store_t::RMW().
    class RmwContext : public IAsyncContext {
    public:
        typedef Key key_t;
        typedef Value value_t;

        RmwContext(uint64_t key, uint64_t incr)
                : key_{ key }
                , incr_{ incr } {
        }

        /// Copy (and deep-copy) constructor.
        RmwContext(const RmwContext& other)
                : key_{ other.key_ }
                , incr_{ other.incr_ } {
        }

        /// The implicit and explicit interfaces require a key() accessor.
        const Key& key() const {
            return key_;
        }
        inline static constexpr uint32_t value_size() {
            return sizeof(value_t);
        }
        inline static constexpr uint32_t value_size(const value_t& old_value) {
            return sizeof(value_t);
        }

        /// Initial, non-atomic, and atomic RMW methods.
        inline void RmwInitial(value_t& value) {
            //value.value_ = incr_;
        }
        inline void RmwCopy(const value_t& old_value, value_t& value) {
            //value.value_ = old_value.value_ + incr_;
        }
        inline bool RmwAtomic(value_t& value) {
            //value.atomic_value_.fetch_add(incr_);
            return true;
        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }

    private:
        Key key_;
        uint64_t incr_;
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

        inline static constexpr uint32_t value_size() {
            return Value::size();
        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }
    };


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
        store2.CompletePending();
        store2.StopSession();


        return(0);
    }


    void runBatchLatency()  override{
        uint64_t idx = 0;
        responseTimes.resize(operationsList.size());
        fasterKeys.resize(keysList.size());
        std::unordered_map<std::string, uint64_t> hashMap;
        uint64_t  keyindex =0;
        valueSize = valuesList[0].size();

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
        //std::cout << "the value size" << valuesList[idx].length() << std::endl;
        while (idx < valuesList.size()) {
            switch (operationsList[idx]) {
                case Operation::putOpr: {
                    startTime = std::chrono::high_resolution_clock::now();

                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        CallbackContext<UpsertContext> context{ ctxt };
                    };
                    UpsertContext context{ fasterKeys[idx], 100};
                    //UpsertContext context{ fasterKeys[idx], (uint8_t *)valuesList[idx].c_str(), (uint32_t)valueSize};
                    Status result = store.Upsert(context, callback, 1);
                    std::cout << "ok "<< (Status::Ok==result) << " put " <<   " val: " <<  11 << " key: " << fasterKeys[idx] << " my key: "  << keysList[idx] <<  std::endl;
                    finishTime = std::chrono::high_resolution_clock::now();
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
                    if(Status::Ok==result) {
                        std::cout << "ok " << (Status::Ok == result) << " get " << " gotten val: "
                                  << context.output_pt1[VALUESIZE - 1] << " key: " << fasterKeys[idx] << " my key"
                                  << keysList[idx] << std::endl;
                    } else {
                        std::cout << "ok "<< (Status::Ok==result) << " get "   << " key: " << fasterKeys[idx] << " my key"  << keysList[idx] <<  std::endl;
                    }
                    /*
                    if(result == Status::Ok)
                    std::cout<<  "result" << "ok" <<std::endl;
                    if(result == Status::NotFound)
                        std::cout<<  "result" << "not found" <<std::endl;
                    if(result == Status::IOError)
                        std::cout<<  "result" << "Io  error" <<std::endl;

                    assert(Status::Ok==result); */
                    finishTime = std::chrono::high_resolution_clock::now();
                    break;
                }
                case Operation::mergeOpr: {
                    startTime = std::chrono::high_resolution_clock::now();
                    //const char *val = valuesList[idx].c_str();
                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        CallbackContext<RmwContext> context{ ctxt };
                    };
                    RmwContext context{  fasterKeys[idx] , 5 };
                    //RmwContext mcontext{ fasterKeys[idx], (uint8_t *)valuesList[idx].c_str(), (uint32_t)valueSize};
                    Status _mresult = store.Rmw(context, callback, 1);
                    finishTime = std::chrono::high_resolution_clock::now();

                    break;
                }

                case Operation::deleteOpr: {
                    startTime = std::chrono::high_resolution_clock::now();
                    auto callback = [](IAsyncContext *ctxt, Status result) {
                        CallbackContext<DeleteContext> context{ctxt};
                    };

                    DeleteContext context{fasterKeys[idx]};
                    Status result = store.Delete(context, callback, 1);
                    std::cout << "ok "<< (Status::Ok==result) << " delete " <<   " val: " <<  valuesList[idx] << "key: " << fasterKeys[idx] << " my key: "  << keysList[idx] <<  std::endl;
                    //assert(Status::Ok == result);
                    finishTime = std::chrono::high_resolution_clock::now();
                    break;

                }
                    /*case Operation::rmwOpr: {
                        // no operations
                        auto callback = [](IAsyncContext* ctxt, Status result) {
                            CallbackContext<RmwContext> context{ ctxt };
                        };

                        RmwContext context{ fasterKeys[idx], 5 };
                        Status result;
                        result = myStore->Rmw(context, callback, 1);
                        if(result == Status::Ok) {
                            ++writes_done;
                        }
                        break;
                    } */
            }
            auto commandExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
            responseTimes[idx] = commandExecutionTime;
            idx ++;
        }

    }

    double runBatchThroughput() override{
        /*
        uint64_t idx = 0;
        responseTimes.resize(operationsList.size());
        fasterKeys.resize(keysList.size());
        std::unordered_map<std::string, uint64_t> hashMap;
        uint64_t  keyindex =0;
        valueSize = valuesList[0].size();

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
        uint64_t  numDeletes = 0; // since faster does not have delete lets remove deletes from throughput calculation
        startTime = std::chrono::high_resolution_clock::now();
        while (idx < valuesList.size()) {
            switch (operationsList[idx]) {
                case Operation::putOpr:
                {
                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        CallbackContext<UpsertContext> context{ ctxt };
                    };
                   // UpsertContext context{ fasterKeys[idx], (uint8_t *)valuesList[idx].c_str(), (uint32_t)valueSize}; // fixme remove the size
                   // UpsertContext context{ fasterKeys[idx], 0 };
                    UpsertContext context{ fasterKeys[idx], 100};
                    Status result = store2.Upsert(context, callback, 1);
                    //++writes_done;
                    //assert(Status::Ok==result);
                }
                    break;
                case Operation::getOpr:
                {
                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        CallbackContext<ReadContext> context{ ctxt };
                    };
                    ReadContext context{fasterKeys[idx]};

                    Status result = store2.Read(context, callback, 1);
                    if(result==Status::Pending) {
                        bool _pendingres = store2.CompletePending(true);
                        if (_pendingres)
                            result = Status::Ok;
                    }


                    //if(result == Status::Ok)
                    //std::cout<<  "result" << "o k" <<std::endl;
                    //if(result == Status::NotFound)
                     //   std::cout<<  "result" << "not found" <<std::endl;
                    //if(result == Status::IOError)
                    //   std::cout<<  "result" << "Io  error" <<std::endl;

                    //assert(Status::Ok==result);
                }
                    break;
                case Operation::mergeOpr:
                {
                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        CallbackContext<RmwContext> context{ ctxt };
                    };

                    RmwContext context{  fasterKeys[idx] , 5 };
                    //RmwContext mcontext{ fasterKeys[idx], (uint8_t *)valuesList[idx].c_str(), (uint32_t)valueSize};
                    Status _mresult = store2.Rmw(context, callback, 1);

                }
                    break;
                case Operation::deleteOpr: {
                    //Key key{ std::make_pair(idx % 7, thread_idx * kNumOps + idx) };

                    auto callback = [](IAsyncContext *ctxt, Status result) {
                        CallbackContext<DeleteContext> context{ctxt};
                    };
                    //ReadContext context{fasterKeys[idx]};
                    DeleteContext context{fasterKeys[idx]};
                    Status result = store2.Delete(context, callback, 1);
                    //std::cout << "ok "<< (Status::Ok==result) << " delete " <<   " val: " <<  valuesList[idx] << "key" << fasterKeys[idx] << " my key"  << keysList[idx] <<  std::endl;
                    //assert(Status::Ok == result);
                    break;
                }
            }
            idx ++;
        }
        finishTime = std::chrono::high_resolution_clock::now();
        auto ExperimentExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
        return (double)operationsList.size()/ (double)ExperimentExecutionTime; // Throughput
        */
        return  0;
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

    void  dumpOperationsOnFile() {}
private:
    std::string connectionString_;
    std::string kDBPath;
    rocksdb::DB* db;
    rocksdb::Options options;
    rocksdb::Status s;
    TimeVar startTime, finishTime;
    std::string response;
    std::vector<uint64_t> fasterKeys;
    store_t* myStore;
    uint32_t  valueSize;
};




#endif //GADGET_FASTER_H