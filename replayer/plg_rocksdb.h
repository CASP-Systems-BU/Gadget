//
// Created by fl on 12/24/20.
//

#include "plg.h"

//#include "rocksdb/c.h"

//#include "rocksdb/db.h"

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "rocksdb/cache.h"
#include "rocksdb/table.h"
#include "rocksdb/merge_operator.h"

#ifndef REPLAYER_PLG_ROCKSDB_H
#define REPLAYER_PLG_ROCKSDB_H

//using namespace rocksdb;


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

    const char* Name() const override { return "MyMerge"; }
};



class plg_rocksdb: public plg{
public:
    int opendb();
    int closedb();

    double opr_insert(uint64_t ikey, const char* val);
    double opr_update(uint64_t ikey, const char* val);
    double opr_read(uint64_t ikey);
    double opr_delete(uint64_t ikey);
    double opr_merge(uint64_t ikey, const char* val);


//    rocksdb_t *db;
//    rocksdb_backup_engine_t *be;
//    rocksdb_options_t *options;
//    rocksdb_writeoptions_t *writeoptions;
//    rocksdb_readoptions_t *readoptions;

    ROCKSDB_NAMESPACE::DB* db;
    ROCKSDB_NAMESPACE::Options options;
    ROCKSDB_NAMESPACE::WriteOptions writeOpt;


};




#endif //REPLAYER_PLG_ROCKSDB_H
