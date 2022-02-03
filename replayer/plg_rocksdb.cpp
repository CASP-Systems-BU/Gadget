//
// Created by fl on 12/24/20.
//

#include "plg_rocksdb.h"

bool assert_opr_success(bool eql, double latency){
    if(!eql){
        throw latency;
    }
    return(eql);
}

int plg_rocksdb::opendb(){
    std::string DBPath = "./tmp/";

    options.create_if_missing = true;       // create the DB if it's not already present
    options.merge_operator.reset(new MyMerge2);
    options.stats_dump_period_sec = 50;

    //add LRU cache. see https://github.com/facebook/rocksdb/wiki/Block-Cache
    std::shared_ptr<ROCKSDB_NAMESPACE::Cache> cache = ROCKSDB_NAMESPACE::NewLRUCache(64*1024*1024);    //64MB
    ROCKSDB_NAMESPACE::BlockBasedTableOptions table_options;
    table_options.block_cache = cache;
    //table_options.block_size = 128 * 1024;    //128KB
    options.table_factory.reset(NewBlockBasedTableFactory(table_options));

    options.write_buffer_size = 128*1024*1024;   //128*2=256MB mem-table size in total, see rocksdb/options.h L200
    options.max_write_buffer_number = 2;          //default value is 2

    //disable WAL:  https://github.com/facebook/rocksdb/wiki/Write-Ahead-Log#writeoptionsdisablewal
    writeOpt = ROCKSDB_NAMESPACE::WriteOptions();
    writeOpt.disableWAL = true;

    ROCKSDB_NAMESPACE::Status s = ROCKSDB_NAMESPACE::DB::Open(options, DBPath, &db);
    assert_opr_success(s.ok(), 0);

    printf("rocksdb opened \n");
    return(0);
}

int plg_rocksdb::closedb(){
    ROCKSDB_NAMESPACE::Status s;
    s = db->Close();
    delete db;
//    ROCKSDB_NAMESPACE::DestroyDB("./tmp/", options);
    printf("rocksdb closed \n");
    return(0);
}

double plg_rocksdb::opr_insert(uint64_t ikey, const char* value){
    char xkey[8];
    memcpy(xkey,&ikey,sizeof(ikey));
    std::string skey=std::string(xkey);
    std::string sval=std::string(value);

    auto tp0 = std::chrono::high_resolution_clock::now();

    ROCKSDB_NAMESPACE::Status si = db->Put(writeOpt, ROCKSDB_NAMESPACE::Slice(skey.data(), skey.size()), ROCKSDB_NAMESPACE::Slice(sval.data(), sval.size()));

    auto tp1 = std::chrono::high_resolution_clock::now();

    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
    //assert_opr_success(si.ok(), tps);
    return(tps);
}

double plg_rocksdb::opr_update(uint64_t ikey, const char* value){    //manually get then put
    char xkey[8];
    memcpy(xkey,&ikey,sizeof(ikey));
    std::string skey=std::string(xkey);
    std::string sval=std::string(value);

    auto tp0 = std::chrono::high_resolution_clock::now();

    std::string value_;
    ROCKSDB_NAMESPACE::Status sg = db->Get(ROCKSDB_NAMESPACE::ReadOptions(), ROCKSDB_NAMESPACE::Slice(skey.data(), skey.size()), &value_);
    ROCKSDB_NAMESPACE::Status sp = db->Put(writeOpt, ROCKSDB_NAMESPACE::Slice(skey.data(), skey.size()), ROCKSDB_NAMESPACE::Slice(sval.data(), sval.size()));

    auto tp1 = std::chrono::high_resolution_clock::now();

    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
    //assert_opr_success(sg.ok(), tps);
    //assert_opr_success(sp.ok(), tps);
    return(tps);
}

double plg_rocksdb::opr_read(uint64_t ikey){
    char xkey[8];
    memcpy(xkey,&ikey,sizeof(ikey));
    std::string skey=std::string(xkey);
    std::string value_;

    auto tp0 = std::chrono::high_resolution_clock::now();

    ROCKSDB_NAMESPACE::Status sr = db->Get(ROCKSDB_NAMESPACE::ReadOptions(), ROCKSDB_NAMESPACE::Slice(skey.data(), skey.size()), &value_);
    //std::cout<<"rocksdb Read==="<<sr.code()<<" "<<sr.subcode()<<"\n";

    auto tp1 = std::chrono::high_resolution_clock::now();

    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
        //std::cout<<"rocskdb read keysize=="<<skey.size()<<" "<<"valsize=="<<value_.size()<<"    "<<skey<<"____"<<value_<<"\n";              //DEBUG
    //assert_opr_success(sr.ok(), tps);
    return(tps);
}

double plg_rocksdb::opr_delete(uint64_t ikey){
    char xkey[8];
    memcpy(xkey,&ikey,sizeof(ikey));
    std::string skey=std::string(xkey);

    auto tp0 = std::chrono::high_resolution_clock::now();

    ROCKSDB_NAMESPACE::Status sd = db->Delete(writeOpt, ROCKSDB_NAMESPACE::Slice(skey.data(), skey.size()));

    auto tp1 = std::chrono::high_resolution_clock::now();

    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
    //assert_opr_success(sd.ok(), tps);
    return(tps);
}

double plg_rocksdb::opr_merge(uint64_t ikey, const char* value){    //build-in RMW operation
    char xkey[8];
    memcpy(xkey,&ikey,sizeof(ikey));
    std::string skey=std::string(xkey);
    std::string sval=std::string(value);

    auto tp0 = std::chrono::high_resolution_clock::now();

    ROCKSDB_NAMESPACE::Status sm = db->Merge(writeOpt, ROCKSDB_NAMESPACE::Slice(skey.data(), skey.size()), ROCKSDB_NAMESPACE::Slice(sval.data(), sval.size()));
    //std::cout<<"rocksdb Merge==="<<sm.code()<<" "<<sm.subcode()<<"\n";

    auto tp1 = std::chrono::high_resolution_clock::now();

    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
    //assert_opr_success(sm.ok(), tps);
    return(tps);

}
