//
// Created by fl on 3/22/21.
//

#include "plg_berkeley.h"

bool assert_opr_success(bool eql, double latency){
    if(!eql){
        throw latency;
    }
    return(eql);
}

int plg_berkeley::opendb(){
    //env.open("/home/fl/Desktop/replayer/tmp/", DB_CREATE | DB_INIT_MPOOL, 0);
    pdb = new Db(NULL, 0);
    //pdb->set_flags(DB_DUP);
    int pstatus=pdb->set_pagesize(1024);    //1024 bytes
    int cstatus=pdb->set_cachesize(0,256*1024*1024,1);    //256MB
    int ostatus=pdb->open(NULL, "./tmp/access.db", NULL, DB_BTREE, DB_CREATE, 0);
    assert_opr_success(ostatus==0, 0);
    assert_opr_success(pstatus==0, 0);
    assert_opr_success(cstatus==0, 0);

    printf("berkeleydb opened \n");
    return(0);
}

int plg_berkeley::closedb(){
    //TODO: don't to any logging
    if (pdb != NULL) {
        pdb->close(0);
        delete pdb;
        // You have to close and delete an exisiting handle, then create a new one before you can use it to remove a database (file).
        pdb = new Db(NULL, 0);
        pdb->remove("./tmp/access.db", NULL, 0);
        //delete pdb;
    }
    printf("berkeleydb closed \n");
    return(0);
}

double plg_berkeley::opr_insert(uint64_t ikey, const char* val){
    unsigned char xkey[8];
    memcpy(xkey,&ikey,sizeof(ikey));

    auto tp0 = std::chrono::high_resolution_clock::now();
    Dbt dkey(const_cast<unsigned char*>(xkey), 8);
    Dbt dval(const_cast<char*>(val), strlen(val));
    int istatus=pdb->put(NULL, &dkey, &dval, 0);
        //std::cout<<"berkeleydb insert keysize=="<< (sizeof(xkey))<<" "<<"valsize=="<<strlen(val)<<"    "<<xkey<<"____"<<val<<"\n";              //DEBUG
    auto tp1 = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
    assert(istatus == 0);
    return(tps);
}

double plg_berkeley::opr_update(uint64_t ikey, const char* val){    //manually get then put
    unsigned char xkey[8];
    memcpy(xkey,&ikey,sizeof(ikey));

    auto tp0 = std::chrono::high_resolution_clock::now();

    Dbt dkey(const_cast<unsigned char*>(xkey), 8);
    Dbt dval;
    std:size_t  oldValueSize;

    int gstatus = pdb->get(NULL, &dkey, &dval, 0);
    if (gstatus == 0) {// successful
        //std::cout << "make the other value" << std::endl;
        // raed it
        oldValueSize = dval.get_size();
        //std::cout << "dval size" << oldValueSize << std::endl;
        //std::cout <<  " dval data: " << (char *)dval.get_data()<< std::endl;
        // append a new value
        char buffer[oldValueSize + VALUESIZE];
        std::memcpy(buffer, dval.get_data(), oldValueSize );
        std::memset(buffer + oldValueSize,'1', VALUESIZE);
        // write the new value
        Dbt dval(buffer, oldValueSize +  VALUESIZE);
        int pstatus =  pdb->put(NULL, &dkey, &dval, 0);
        assert(pstatus == 0);
        //std::cout << " --------------- "  << std::endl;
    } else { // this is the first merge
        //std::cout << "make the firts value" << std::endl;
        char buffer[VALUESIZE];
        std::memset(buffer, '0', VALUESIZE);
        Dbt dval(buffer, VALUESIZE);
        int pstatus =  pdb->put(NULL, &dkey, &dval, 0);
        assert(pstatus == 0);
        //std::cout << "---------------"  << std::endl;
    }

    auto tp1 = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
//assert_opr_success(ustatus!=DB_NOTFOUND, tps);
    return(tps);
}

double plg_berkeley::opr_read(uint64_t ikey){
    unsigned char xkey[8];
    memcpy(xkey,&ikey,sizeof(ikey));

    auto tp0 = std::chrono::high_resolution_clock::now();
    std::size_t  oldValueSize;
    Dbt dkey(const_cast<unsigned char*>(xkey), 8);
    Dbt dval;
        //std::cout<<"berkeleydb read keysize=="<< (sizeof(xkey))<<" "<<"valsize=="<<sizeof(buffer)<<"    "<<xkey<<"____"<<buffer<<"\n";              //DEBUG
    int rstatus = pdb->get(NULL, &dkey, &dval, 0);
    oldValueSize = dval.get_size();
    //std::cout << "the value size : " <<  oldValueSize <<  std::endl;
    char buffer[oldValueSize];
    std::memcpy(buffer, dval.get_data(), oldValueSize );
//    if (status == DB_NOTFOUND) {
//        cout << "Not found" << endl;
//    } else {
//        cout << "Found: " << buffer << endl;
//    }
        //std::cout<<"berkeleydb read keysize=="<< (sizeof(xkey))<<" "<<"valsize=="<<sizeof(buffer)<<"    "<<xkey<<"____"<<buffer<<"\n";              //DEBUG
    auto tp1 = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
//assert_opr_success(rstatus!=DB_NOTFOUND, tps);
    return(tps);
}

double plg_berkeley::opr_delete(uint64_t ikey){
    unsigned char xkey[8];
    memcpy(xkey,&ikey,sizeof(ikey));

    auto tp0 = std::chrono::high_resolution_clock::now();
    Dbt dkey(const_cast<unsigned char*>(xkey), 8);
    int dstatus = pdb->del(NULL, &dkey, 0);
    auto tp1 = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds time_span = tp1 - tp0;
    double tps = (double)time_span.count()/1000;    // us
//assert_opr_success(dstatus!=DB_NOTFOUND, tps);
    return(tps);
}

double plg_berkeley::opr_merge(uint64_t ikey, const char* val){    //build-in RMW operation
    return(opr_update(ikey, val));
    // No built-in RMW in berkeleyDB
    // https://docs.oracle.com/cd/E17275_01/html/api_reference/C/db.html
}
