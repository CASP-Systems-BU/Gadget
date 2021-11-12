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


    class Value {
    public:
        Value()
                : length_{ 0 } {
        }

        inline uint32_t size() const {
            return length_;
        }

        friend class RmwContext;
        friend class UpsertContext;
        friend class ReadContext;

    private:
        uint32_t length_;

        const char* buffer() const {
            return reinterpret_cast<const char*>(this + 1);
        }
        char* buffer() {
            return reinterpret_cast<char*>(this + 1);
        }
    };



    class RmwContext : public IAsyncContext {
    public:
        typedef Key key_t;
        typedef Value value_t;

        RmwContext(uint64_t key, char letter)
                : key_{ key }
                , letter_{ letter } {
        }

        /// Copy (and deep-copy) constructor.
        RmwContext(const RmwContext& other)
                : key_{ other.key_ }
                , letter_{ other.letter_ } {
        }

        /// The implicit and explicit interfaces require a key() accessor.
        inline const Key& key() const {
            return key_;
        }
        inline uint32_t value_size() const {
            return sizeof(value_t) + sizeof(char);
        }
        inline uint32_t value_size(const Value& old_value) const {
            return sizeof(value_t) + old_value.length_ + sizeof(char);
        }

        inline void RmwInitial(Value& value) {
            // value.length_ = sizeof(char);
            value.length_ = VALUESIZE;
            //std::cout << "in rmw  initial val " << value.length_ << std::endl;
            std::memset(value.buffer(), 'p', VALUESIZE);
            //value.buffer()[0] = letter_;
        }
        inline void RmwCopy(const Value& old_value, Value& value) {
            //std::cout << "in rmw  old value length " << old_value.length_ << std::endl;
            value.length_ = old_value.length_ + VALUESIZE;
            //std::cout << "in rmw  new value length " << value.length_ << std::endl;
            //std::memcpy(value.buffer(), old_value.buffer(), old_value.length_);
            //std::cout << "old_value.length_" << old_value.length_ << std::endl;
            std::memcpy(value.buffer(), old_value.buffer(), old_value.length_);
            std::memset(value.buffer() + old_value.length_, 'p', VALUESIZE );
        }
        inline bool RmwAtomic(Value& value) {
            // All RMW operations use Read-Copy-Update
            return false;
        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }

    private:
        char letter_;
        //uint8_t  my_letter[VALUESIZE];
        Key key_;
    };
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
            return sizeof(Value) + length_;
        }
        /// Non-atomic and atomic Put() methods.
        inline void Put(Value& value) {

            value.length_ = length_;
            // std::cout << "value length- upsert" << value.length_ << std::endl;
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
            output_length = value.length_;
            //std::cout << "in read context - non atomic : value.length_ " << value.length_ << std::endl;
            // fixme - We only read the first and the last  letter
            output_letters[0] = value.buffer()[0];
            output_letters[1] = value.buffer()[output_length -1];
        }
        inline void GetAtomic(const Value& value) {
            // There are no concurrent updates
            output_length = value.length_;
            //std::cout << "in read context : value.length_ " << value.length_ << std::endl;
            // fixme - We only read the first and the last  letter
            output_letters[0] = value.buffer()[0];
            output_letters[1] = value.buffer()[output_length -1];
        }

    protected:
        /// The explicit interface requires a DeepCopy_Internal() implementation.
        Status DeepCopy_Internal(IAsyncContext*& context_copy) {
            return IAsyncContext::DeepCopy_Internal(*this, context_copy);
        }

    private:
        Key key_;
    public:
        uint32_t output_length;
        // Extract two letters of output.
        char output_letters[2];
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
            return 10; // Shown - Fixme
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
        // uintTest(); Turn this line on for some tests
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




    double runBatchThroughput() override{

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
        uint_least64_t  loopSize = keysList.size();
        startTime = std::chrono::high_resolution_clock::now();
        while (idx < loopSize) {
            switch (operationsList[idx]) {
                case Operation::putOpr:
                {
                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        CallbackContext<UpsertContext> context{ ctxt };
                    };
                    UpsertContext context{ fasterKeys[idx], VALUESIZE};
                    Status result = store2.Upsert(context, callback, 1);
                    if(result==Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                        bool _pendingres = store2.CompletePending(true);
                        if (_pendingres)
                            result = Status::Ok;
                    }
                    //std::cout << "ok "<< (Status::Ok==result) << " put " <<   " val: " <<  11 << " key: " << fasterKeys[idx] << " my key: "  << keysList[idx] <<  std::endl;
                    break;

                }
                case Operation::getOpr:
                {
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
                    //if(result == Status::Ok)
                    //std::cout<<  "result" << "o k" <<std::endl;
                    //if(result == Status::NotFound)
                    //   std::cout<<  "result" << "not found" <<std::endl;
                    //if(result == Status::IOError)
                    //   std::cout<<  "result" << "Io  error" <<std::endl;

                    //assert(Status::Ok==result);
                    break;
                }
                case Operation::mergeOpr:
                {
                    auto callback = [](IAsyncContext* ctxt, Status result) {
                        //In-memory test.
                        // ASSERT_TRUE(false);
                    };
                    RmwContext context{fasterKeys[idx] , 's' };
                    Status result = store2.Rmw(context, callback, 1);
                    if(result==Status::Pending) { //TODO(showan) explore this maybe we can move this to end of the experiment
                        bool _pendingres = store2.CompletePending(true);
                        if (_pendingres)
                            result = Status::Ok;
                    }
                    //std::cout << "ok "<< (Status::Ok==result) << " rmw "   << " key: " << fasterKeys[idx] << " my key"  << keysList[idx] <<  std::endl;
                    break;
                }

                case Operation::deleteOpr: {
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
                    break;
                }
            }
            idx ++;
        }
        finishTime = std::chrono::high_resolution_clock::now();
        auto ExperimentExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
        std::cout << "ExperimentExecutionTime: " << ExperimentExecutionTime << " operationsList.size:  " << operationsList.size() << std::endl;
        // fixme - should we add complete pending here  e.g.,  store2.CompletePending(true);
        return (double)operationsList.size()/ (double)ExperimentExecutionTime; // Throughput
    }



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
