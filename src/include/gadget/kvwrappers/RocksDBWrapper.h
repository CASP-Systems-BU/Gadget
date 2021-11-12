//
// Created by Showan Asyabi on 1/22/21.
//

#ifndef GADGET_ROCKSDBWRAPPER_H
#define GADGET_ROCKSDBWRAPPER_H
#include <memory>
#include <string>
#include <vector>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "rocksdb/cache.h"
#include "rocksdb/table.h"
#include  "rocksdb/merge_operator.h"





#include "include/gadget/performanceMetrics.h"
#include "include/gadget/kvwrappers/kvwrapper.h"
typedef std::chrono::high_resolution_clock::time_point TimeVar;

// this class is from rocksDB github. They use this class  for merge
class MyMerge2 : public ROCKSDB_NAMESPACE::AssociativeMergeOperator {
public:
  virtual  bool Merge(const rocksdb::Slice& /*key*/,
                                     const rocksdb::Slice* existing_value,
                                     const rocksdb::Slice& value, std::string* new_value,
                      rocksdb::Logger* /*logger*/) const {
      //std::cout << "in merge" << std::endl;
        // Clear the *new_value for writing.
        assert(new_value);
        new_value->clear();

        if (!existing_value) {
            // No existing_value. Set *new_value = value
            new_value->assign(value.data(), value.size());
        } else {
            // Generic append (existing_value != null).
            // Reserve *new_value to correct size, and apply concatenation.
            new_value->reserve(existing_value->size() + 1 + value.size());
            new_value->assign(existing_value->data(), existing_value->size());
            new_value->append(",");
            new_value->append(value.data(), value.size());
        }

        return true;
    }
    virtual const char* Name() const override { return "mymerge 2"; }


    };

class MyMerge : public ROCKSDB_NAMESPACE::MergeOperator {
public:

    virtual bool FullMergeV2(const MergeOperationInput& merge_in,
                             MergeOperationOutput* merge_out) const override {
        merge_out->new_value.clear();
        if (merge_in.existing_value != nullptr) {
            merge_out->new_value.assign(merge_in.existing_value->data(),
                                        merge_in.existing_value->size());
        }
        for (const ROCKSDB_NAMESPACE::Slice& m : merge_in.operand_list) {
            //fprintf(stderr, "Merge(%s)\n", m.ToString().c_str());
            // the compaction filter filters out bad values
            //assert(m.ToString() != "bad");
            merge_out->new_value.assign(m.data(), m.size());
        }
        return true;
    }
/*
    virtual bool FullMergeV2(const MergeOperationInput& merge_in,
                               MergeOperationOutput* merge_out) const {
        std::cout << "full merge" << std::endl;
        std::vector<std::string> left;
        left.clear();
        for (rocksdb::Slice slice : merge_in.operand_list) {
            std::vector<std::string> right;
            MakeVector(right, slice);
            left = Merge(left, right);
        }
        for (int i = 0; i < static_cast<int>(left.size()) - 1; i++) {
            merge_out->new_value.append(left[i]).append(",");
        }
            merge_out->new_value.append(left.back());
        std::cout << "full merge - value" << merge_out->new_value << std::endl;
        return true;
    }

    virtual  bool PartialMerge(const rocksdb::Slice& , const rocksdb::Slice& left_operand,
                                const rocksdb::Slice& right_operand, std::string* new_value,
                               rocksdb::Logger* ) const {
        std::cout << "partial merge" << std::endl;
        std::vector<std::string> left;
        std::vector<std::string> right;
        MakeVector(left, left_operand);
        MakeVector(right, right_operand);
        left = Merge(left, right);
        for (int i = 0; i < static_cast<int>(left.size()) - 1; i++) {
            new_value->append(left[i]).append(",");
        }
        new_value->append(left.back());
        return true;
    }

    virtual bool PartialMergeMulti(const rocksdb::Slice& ,
                                     const std::deque<rocksdb::Slice>& operand_list,
                                     std::string* new_value,
                                   rocksdb::Logger* ) const {
        (void)operand_list;
        (void)new_value;
        return true;
    }

    void MakeVector(std::vector<std::string>& operand, rocksdb::Slice slice) const {
        do {
            const char* begin = slice.data_;
            while (*slice.data_ != ',' && *slice.data_) slice.data_++;
            operand.push_back(std::string(begin, slice.data_));
        } while (0 != *slice.data_++);
    }

    std::vector<std::string> Merge(std::vector<std::string>& left,
                                     std::vector<std::string>& right) const {
        std::cout << " merge" << std::endl;
        // Fill the resultant vector with sorted results from both vectors
        std::vector<std::string> result;
        unsigned left_it = 0, right_it = 0;

       //while (left_it < left.size() && right_it < right.size()) {
            // If the left value is smaller than the right it goes next
            // into the resultant vector
            //if (left[left_it] < right[right_it]) {
             //   result.push_back(left[left_it]);
             //   left_it++;
            //} else {
             //   result.push_back(right[right_it]);
             //   right_it++;
            //}
        //}

        // Push the remaining data from both vectors onto the resultant
        while (left_it < left.size()) {
            result.push_back(left[left_it]);
            left_it++;
        }

        while (right_it < right.size()) {
            result.push_back(right[right_it]);
            right_it++;
        }

        return result;
    }
 */

    const char* Name() const override { return "MyMerge"; }
};


class RocksDBWrapper: public KVWrapper {
public:
    explicit RocksDBWrapper(std::string filePath) {
        kDBPath = filePath;
        responseTime = 0;
    }

    bool connect() override {
        //options.IncreaseParallelism();
        //options.OptimizeLevelStyleCompaction();
        // create the DB if it's not already present
        // Yuanli's config

        options.create_if_missing = true;       // create the DB if it's not already present
        options.merge_operator.reset(new MyMerge2);
        //options.merge_operator =  new  rocksdb::MergeOperators::CreatePutOperator();
        options.stats_dump_period_sec = 50;

        //add LRU cache. see https://github.com/facebook/rocksdb/wiki/Block-Cache
        std::shared_ptr<rocksdb::Cache> cache = rocksdb::NewLRUCache(64*1024*1024);    //64MB
        rocksdb::BlockBasedTableOptions table_options;
        table_options.block_cache = cache;
        //table_options.block_size = 128 * 1024;    //128KB
        options.table_factory.reset(NewBlockBasedTableFactory(table_options));

        options.write_buffer_size = 128*1024*1024;   //256MB mem-table size in total, see rocksdb/options.h L200
        options.max_write_buffer_number = 2;          //default value is 2

        //disable WAL:  https://github.com/facebook/rocksdb/wiki/Write-Ahead-Log#writeoptionsdisablewal
        writeOpt = rocksdb::WriteOptions();
        writeOpt.disableWAL = true;


        // end of  Yuanli's config
        //options.create_if_missing = true;
        rocksdb::Status s;
        s = rocksdb::DB::Open(options, kDBPath, &db);
        if(! s.ok()) {
            std::cout << "RocksDB  Error: " << s.ToString() << std::endl;
        } else {
            std::cout << "Connected  to RocksDB successfully " << s.ToString() << std::endl;
        }
        //uintTest(); Turn this line on to some tests
        return s.ok();
    }

    bool disconnect() override {
        rocksdb::Status s;
        s = db->Close();
        delete db;
        rocksdb::DestroyDB(kDBPath, options);
        std::cout << "RocksDB is closed and destroyed!!? " << s.ok() << std::endl;
        return s.ok();
    }
    void runBatchLatency()  override{
        uint64_t counter = 0;
        responseTimes.resize(operationsList.size());
        while (counter < keysList.size()) {
            switch (operationsList[counter]) {
                case Operation::putOpr: {
                    //std::cout << "put  key" << keysList[counter]  << " key size: " <<  keysList[counter].size()  << " value size: " <<  valuesList[counter].size() << std::endl;

                    startTime = std::chrono::high_resolution_clock::now();
                    auto s = db->Put(writeOpt,
                                     rocksdb::Slice(keysList[counter].data(), keysList[counter].size()),
                                     rocksdb::Slice(valuesList[counter].data(), valuesList[counter].size()));
                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(s.ok());
                    break;
                }
                case Operation::getOpr: {
                    //std::cout << "get " << keysList[counter] << " key size: "<< keysList[counter].size() << " value size: " << valuesList[counter].size() << std::endl;
                    startTime = std::chrono::high_resolution_clock::now();
                    db->Get(rocksdb::ReadOptions(), rocksdb::Slice(keysList[counter].data(), keysList[counter].size()),
                            &response);
                    finishTime = std::chrono::high_resolution_clock::now();
                    //std::cout << response.size()<< std::endl;
                    response.clear();
                    break;
                }
                case Operation::mergeOpr: {
                    //std::cout << "merge " << keysList[counter] <<   "key size:" <<   keysList[counter].size() << " value size: " << valuesList[counter].size() << std::endl;
                    startTime = std::chrono::high_resolution_clock::now();
                    //std::cout << "key" << keysList[counter]  << "merge" << valuesList[counter] <<std::endl;
                    auto s = db->Merge(writeOpt,
                              rocksdb::Slice(keysList[counter].data(), keysList[counter].size()),
                              rocksdb::Slice(valuesList[counter].data(), valuesList[counter].size()));
                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(s.ok());
                    break;
                }
                case Operation::deleteOpr: {
                    //std::cout << "delete " << keysList[counter] <<  "key size:" <<   keysList[counter].size() << " value size: "  << valuesList[counter].size() << std::endl;
                    // std::cout <<"delete" <<std::endl;
                    startTime = std::chrono::high_resolution_clock::now();
                    auto s =   db->Delete(writeOpt,
                               rocksdb::Slice(keysList[counter].data(), keysList[counter].size()));
                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(s.ok());
                    break;
                }
            }
            auto commandExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
            responseTimes[counter] = commandExecutionTime;
            //std::cout << "commandexetime: " << commandExecutionTime << std::endl;
            counter ++;
            //std::cout << "**************************************: " << counter << std::endl;
        }
    }

    double runBatchThroughput() override{
        TimeVar mystartTime, myfinishTime;
        assert(valuesList.size() > 0);
        assert(valuesList.size() == keysList.size());
        assert(valuesList.size() == operationsList.size());
        uint64_t counter = 0;
        startTime = std::chrono::high_resolution_clock::now();
        mystartTime = startTime;
        while (counter < keysList.size()) {

            switch (operationsList[counter]) {
                case Operation::putOpr: {
                     db->Put(writeOpt,
                                                rocksdb::Slice(keysList[counter].data(), keysList[counter].size()),
                                                rocksdb::Slice(valuesList[counter].data(), valuesList[counter].size()));
                    break;
                }

                case Operation::getOpr: {

                       db->Get(rocksdb::ReadOptions(), rocksdb::Slice(keysList[counter].data(), keysList[counter].size()),
                                                 &response);
                    //std::cout << "response: "<< response<< std::endl;
                    //std::cout << "key" <<keysList[counter]<<std::endl;
                    break;
                }
                case Operation::mergeOpr: {
                    //std::cout << "key" << keysList[counter]  << "merge" << valuesList[counter] <<std::endl;
                   auto s = db->Merge(writeOpt, rocksdb::Slice(keysList[counter].data(), keysList[counter].size()), rocksdb::Slice(valuesList[counter].data(), valuesList[counter].size()));
                   //std::cout << s.ok() << std::endl;
                    break;
                }
                case Operation::deleteOpr: {
                     //std::cout <<"delete" <<std::endl;
                     db->Delete(writeOpt,
                                                   rocksdb::Slice(keysList[counter].data(), keysList[counter].size()));
                    break;
                }
            }
            counter ++;
            if (counter == 10000) {
                myfinishTime = std::chrono::high_resolution_clock::now();
                auto myExperimentExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( myfinishTime - mystartTime ).count();
                mystartTime = std::chrono::high_resolution_clock::now();
               //std::cout << "myThr: " << (10000/ (double)myExperimentExecutionTime) * 1000000 << std::endl;

            }
        }
        finishTime = std::chrono::high_resolution_clock::now();
        auto ExperimentExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
        return (double)operationsList.size()/ (double)ExperimentExecutionTime; // Throughput

    }

   void  uintTest() {

        std::string key= "key1";
        std::string value = "value1";


        //get
        {
            auto s =  db->Get(rocksdb::ReadOptions(), rocksdb::Slice(key.data(), key.size()),
                    &response);
            assert(!s.ok());
        }
        // put
        {
            auto s = db->Put(rocksdb::WriteOptions(), rocksdb::Slice(key.data(), key.size()), rocksdb::Slice(value.data(), value.size()));
            assert(s.ok());
        }
        // get
        {
            auto s =  db->Get(rocksdb::ReadOptions(), rocksdb::Slice(key.data(), key.size()),
                             &response);
            std::cout << "first response" << response << std::endl;
            assert(s.ok());
        }
        // put
        {
            value= "new bigger value" ;
            auto s = db->Put(rocksdb::WriteOptions(), rocksdb::Slice(key.data(), key.size()), rocksdb::Slice(value.data(), value.size()));

            assert(s.ok());

        }

        // get
        {
            auto s =  db->Get(rocksdb::ReadOptions(), rocksdb::Slice(key.data(), key.size()),
                              &response);
            std::cout << "second response" << response << std::endl;
            assert(s.ok());

        }
        // delete
        {
            db->Delete(rocksdb::WriteOptions(),
                       rocksdb::Slice(key.data(), key.size()));
        }
        // get
        {

            auto s =  db->Get(rocksdb::ReadOptions(), rocksdb::Slice(key.data(), key.size()),
                              &response);
            std::cout << "response  after delete" << response << std::endl;
            assert(!s.ok());

        }

        // rmw

        {
            value = "hi";
            auto s = db->Merge(rocksdb::WriteOptions(), rocksdb::Slice(key.data(), key.size()), rocksdb::Slice(value.data(), value.size()));
            assert(s.ok());
        }
        // rmw
        {
            value = "how ";
            auto s = db->Merge(rocksdb::WriteOptions(), rocksdb::Slice(key.data(), key.size()), rocksdb::Slice(value.data(), value.size()));
            assert(s.ok());
        }
        // rmw
        {
            value = "are you";
            auto s = db->Merge(rocksdb::WriteOptions(), rocksdb::Slice(key.data(), key.size()), rocksdb::Slice(value.data(), value.size()));
            assert(s.ok());

        }
        // get
        {

            auto s =  db->Get(rocksdb::ReadOptions(), rocksdb::Slice(key.data(), key.size()),
                              &response);
            std::cout << "response  after merges" << response << std::endl;
            assert(s.ok());
        }
       // delete
       {
           auto s = db->Delete(rocksdb::WriteOptions(),
                      rocksdb::Slice(key.data(), key.size()));
           assert(s.ok());
       }
       // get
       {

           auto s =  db->Get(rocksdb::ReadOptions(), rocksdb::Slice(key.data(), key.size()),
                             &response);
           std::cout << "response  after delete" << response << std::endl;
           assert(!s.ok());

       }

        assert(1 == 2);
    }

    void  dumpOperationsOnFile() {}
private:
    std::string kDBPath;
    rocksdb::DB* db;
    rocksdb::Options options;
    rocksdb::WriteOptions writeOpt;
    TimeVar startTime, finishTime;
    std::string response;
};
#endif //GADGET_ROCKSDBWRAPPER_H