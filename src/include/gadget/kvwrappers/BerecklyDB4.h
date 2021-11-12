//
// Created by Showan Asyabi on 4/21/21.
//

#ifndef GADGET_BEREKLY_H
#define GADGET_BEREKLY_H

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
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>


#include "include/gadget/performanceMetrics.h"
#include "include/gadget/kvwrappers/kvwrapper.h"
typedef std::chrono::high_resolution_clock::time_point TimeVar;


#include <db_cxx.h>





class BerkeleyDBWrapper: public KVWrapper {
public:
    explicit BerkeleyDBWrapper(std::string path) {
        //connectionString_ = connectionString;
        //TODO fixme
        //kDBPath = "/Users/showanasyabi/Desktop/trash";
        kDBPath = path;
        responseTime = 0;
        dupIsEnabled = false;
        mergeExist = false;
    }

    bool connect() override {
        pdb = new Db(NULL, 0);
        file_name = "easyabi.db"; // it seems it has problem with name "access.db" when duplicate is turned on

        // turn this line on for holistic operators
        //pdb->set_flags(DB_DUP);
        //if(mergeExist) {
        //    pdb->set_flags(DB_DUP);
         //   std::cout << "merge exist" << std::endl;
        //}
        int pstatus = pdb->set_pagesize(1024);    //1024 bytes
        int cstatus = pdb->set_cachesize(0, 256 * 1024 * 1024,
                                         1);    //256 // fixme we use 256 for BerkeleyDB while we use 256+64 for Faster- Maybe this is fair because the tree takes space in bereckyDB
        int ostatus = pdb->open(NULL, file_name, NULL, DB_BTREE, DB_CREATE, 0);


        //assert(ostatus==0);
        //assert(pstatus==0);
        //assert(cstatus==0);
        //assert(dupstatus == 0);

        std::cout << "Connected to BerkeleyDB successfully" << std::endl;
        //unitTest();
        return true;
    }

    bool disconnect() override {
        if (pdb != NULL) {
            pdb->close(0);
            delete pdb;
            // You have to close and delete an existing handle, then create a new one before you can use it to remove a database (file).
            pdb = new Db(NULL, 0);
            pdb->remove(file_name, NULL, 0);
            //delete pdb;
        }
        printf("BerkeleyDB  is closed \n");
        return true;
    }




    void runBatchLatency()  override { // new version - merge: get-append-put it is faster than dup keys

        uint64_t idx = 0;
        uint64_t  valSize = (valuesList[0]).length();;
        uint64_t  keySize = (keysList[0]).length();
        uint64_t  loopSize =   keysList.size();
        responseTimes.resize(operationsList.size());
        assert(valuesList.size() == keysList.size());
        assert(operationsList.size() == keysList.size());
        int status = 0;

        while (idx < loopSize) {
            switch (operationsList[idx]) {
                case Operation::putOpr: {
                    //unsigned char xkey[sizeof(keysList[idx])];
                    //std::copy( keysList[idx].begin(), keysList[idx].end(), xkey );
                    startTime = std::chrono::high_resolution_clock::now();
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    Dbt dval(const_cast<char*>(valuesList[idx].c_str()), valSize + 1);
                    status = pdb->put(NULL, &dkey, &dval, 0);
                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(status == 0);

                    break;
                }
                case Operation::getOpr: {
                    startTime = std::chrono::high_resolution_clock::now();

                    std::size_t  oldValueSize;
                    Dbt dkey(const_cast<char *>(keysList[idx].c_str()), keySize + 1);
                    Dbt dval;
                    status = pdb->get(NULL, &dkey, &dval, 0);
                    oldValueSize = dval.get_size();
                    //std::cout << "the value size : " <<  oldValueSize <<  std::endl;
                    char buffer[oldValueSize];
                    std::memcpy(buffer, dval.get_data(), oldValueSize );
                    //assert(status == 0); // this might return false for incremntal operators (i.e., get put)

                    finishTime = std::chrono::high_resolution_clock::now();
                    break;
                }

                case Operation::mergeOpr: // we translate merge to a get and a put operation
                {
                    startTime = std::chrono::high_resolution_clock::now();

                    Dbt dkey(const_cast<char *>(keysList[idx].c_str()), keySize + 1);
                    Dbt dval;
                    std:size_t  oldValueSize;

                    status = pdb->get(NULL, &dkey, &dval, 0);
                    if (status == 0) {// successful
                        //std::cout << "make the other value" << std::endl;
                        // raed it
                        oldValueSize = dval.get_size();
                        //std::cout << "dval size" << oldValueSize << std::endl;
                        //std::cout <<  " dval data: " << (char *)dval.get_data()<< std::endl;

                        // append a new value
                        char buffer[oldValueSize + valSize];
                        std::memcpy(buffer, dval.get_data(), oldValueSize );
                        std::memset(buffer + oldValueSize,'1', valSize);

                        // write the new value
                        Dbt dval(buffer, oldValueSize +  valSize);
                        status =  pdb->put(NULL, &dkey, &dval, 0);
                        assert(status == 0);
                        //std::cout << " --------------- "  << std::endl;

                    } else { // this is the first merge
                        //std::cout << "make the firts value" << std::endl;
                        Dbt dkey(const_cast<char *>(keysList[idx].c_str()), keySize + 1);
                        char buffer[valSize];
                        std::memset(buffer, '0', valSize);
                        Dbt dval(buffer, valSize);
                        status =  pdb->put(NULL, &dkey, &dval, 0);
                        assert(status == 0);
                        //std::cout << "---------------"  << std::endl;
                    }



                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(status == 0);
                    break;
                }
                case Operation::deleteOpr:
                {
                    startTime = std::chrono::high_resolution_clock::now();
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    status = pdb->del(NULL, &dkey, 0);
                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(status == 0);
                    break;
                }

            }
            auto commandExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
            responseTimes[idx] = commandExecutionTime;
            idx ++;
            //if(idx % 100000 == 0) { // fixme remove
            //    std::cout<< idx <<  " out of "<< loopSize <<std::endl;
            // }
        }


    }


/*
    void runBatchLatency()  override {

        uint64_t idx = 0;
        uint64_t  valSize = (valuesList[0]).length();;
        uint64_t  keySize = (keysList[0]).length();
        uint64_t  loopSize =   keysList.size();
        responseTimes.resize(operationsList.size());
        assert(valuesList.size() == keysList.size());
        assert(operationsList.size() == keysList.size());
        int status = 0;

        while (idx < loopSize) {
            switch (operationsList[idx]) {
                case Operation::putOpr: {
                    //unsigned char xkey[sizeof(keysList[idx])];
                    //std::copy( keysList[idx].begin(), keysList[idx].end(), xkey );
                    startTime = std::chrono::high_resolution_clock::now();
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    Dbt dval(const_cast<char*>(valuesList[idx].c_str()), valSize + 1);
                    status = pdb->put(NULL, &dkey, &dval, 0);
                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(status == 0);

                    break;
                }
                case Operation::getOpr: {
                    //unsigned char xkey[sizeof(keysList[idx])];
                    //uint64_t keySize = keysList[idx].length();
                    //std::copy( keysList[idx].begin(), keysList[idx].end(), xkey );
                    //memcpy(xkey,&keysList[idx],sizeof(keysList[idx]));
                    startTime = std::chrono::high_resolution_clock::now();
                    char buffer[valSize + 1];
                    if(!mergeExist) {
                        Dbt dkey(const_cast<char *>(keysList[idx].c_str()), keySize + 1);
                        Dbt dval;
                        dval.set_data(buffer);
                        dval.set_ulen(valSize + 1);
                        dval.set_flags(DB_DBT_USERMEM);
                        status = pdb->get(NULL, &dkey, &dval, 0);
                        assert(status == 0);
                    } else {

                        Dbc *cursorp;
                        pdb->cursor(NULL, &cursorp, 0);
                        Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                        Dbt data;
                        int ret = cursorp->get(&dkey, &data, DB_SET);
                        //int i = 0;
                        while (ret != DB_NOTFOUND) {
                            //  i++;
                            ret = cursorp->get(&dkey, &data, DB_NEXT_DUP);
                        }
                        //std::cout << "while size: " << i << std::endl;

                    }
                    finishTime = std::chrono::high_resolution_clock::now();
                    break;
                }

                case Operation::mergeOpr: // we translate merge to a get and a put operation
                {
                    startTime = std::chrono::high_resolution_clock::now();
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    Dbt dval(const_cast<char*>(valuesList[idx].c_str()), valSize + 1);
                    status =  pdb->put(NULL, &dkey, &dval, 0);
                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(status == 0);
                    break;
                }
                case Operation::deleteOpr:
                {
                    startTime = std::chrono::high_resolution_clock::now();
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    status = pdb->del(NULL, &dkey, 0);
                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(status == 0);
                    break;
                }

            }
            auto commandExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
            responseTimes[idx] = commandExecutionTime;
            idx ++;
            //if(idx % 100000 == 0) { // fixme remove
            //    std::cout<< idx <<  " out of "<< loopSize <<std::endl;
            // }
        }


    }
*/



    double runBatchThroughput() override{  // second edition : merge : get append put

        assert(valuesList.size() > 0);
        assert(valuesList.size() == keysList.size());
        assert(valuesList.size() == operationsList.size());
        uint64_t idx = 0;
        uint64_t  valSize = valuesList[0].size();
        uint64_t  keySize = (keysList[0]).length();
        uint64_t  loopSize = keysList.size();
        responseTimes.resize(operationsList.size());
        int status = 0;
        startTime = std::chrono::high_resolution_clock::now();
        while (idx < loopSize ) {
            switch (operationsList[idx]) {
                case Operation::putOpr: {
                    //startTime2 = std::chrono::high_resolution_clock::now();
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    Dbt dval(const_cast<char*>(valuesList[idx].c_str()), valSize + 1);
                    status = pdb->put(NULL, &dkey, &dval, 0);
                    assert(status == 0);
                    //finishTime2 = std::chrono::high_resolution_clock::now();
                    //auto Oprduration = std::chrono::duration_cast<std::chrono::microseconds>( finishTime2 - startTime2 ).count();
                    //std::cout << "put duration: : " << Oprduration << std::endl;
                    break;
                }
                case Operation::getOpr: {
                    //if(!mergeExist) {
                        std::size_t  oldValueSize;
                        Dbt dkey(const_cast<char *>(keysList[idx].c_str()), keySize + 1);
                        Dbt dval;
                        status = pdb->get(NULL, &dkey, &dval, 0);
                        oldValueSize = dval.get_size();
                        //std::cout << "the value size : " <<  oldValueSize <<  std::endl;
                        char buffer[oldValueSize];
                        std::memcpy(buffer, dval.get_data(), oldValueSize );
                        //assert(status == 0); // this might return false for incremntal operators (i.e., get put)

                    break;
                }

                case Operation::mergeOpr:
                {


                    // make the key
                    Dbt dkey(const_cast<char *>(keysList[idx].c_str()), keySize + 1);
                    Dbt dval;
                    std:size_t  oldValueSize;

                    status = pdb->get(NULL, &dkey, &dval, 0);
                    if (status == 0) {// successful
                        //std::cout << "make the other value" << std::endl;
                        // raed it
                        oldValueSize = dval.get_size();
                        //std::cout << "dval size" << oldValueSize << std::endl;
                        //std::cout <<  " dval data: " << (char *)dval.get_data()<< std::endl;

                        // append a new value
                        char buffer[oldValueSize + valSize];
                        std::memcpy(buffer, dval.get_data(), oldValueSize );
                        std::memset(buffer + oldValueSize,'1', valSize);

                        // write the new value
                        Dbt dval(buffer, oldValueSize +  valSize);
                        status =  pdb->put(NULL, &dkey, &dval, 0);
                        assert(status == 0);
                        //std::cout << " --------------- "  << std::endl;

                    } else { // this is the first merge
                        //std::cout << "make the firts value" << std::endl;
                        Dbt dkey(const_cast<char *>(keysList[idx].c_str()), keySize + 1);
                        char buffer[valSize];
                        std::memset(buffer, '0', valSize);
                        Dbt dval(buffer, valSize);
                        status =  pdb->put(NULL, &dkey, &dval, 0);
                        assert(status == 0);
                        //std::cout << "---------------"  << std::endl;
                    }


                    break;

                }
                case Operation::deleteOpr: {
                    //startTime2 = std::chrono::high_resolution_clock::now();
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    status = pdb->del(NULL, &dkey, 0);
                    assert(status == 0);
                    //finishTime2 = std::chrono::high_resolution_clock::now();
                    //auto Oprduration = std::chrono::duration_cast<std::chrono::microseconds>( finishTime2 - startTime2 ).count();
                    //std::cout << "delete  duration: : " << Oprduration << std::endl;
                    break;

                }
            }

            idx ++;
            //if(idx % 10000 == 0)
              //  std::cout << "idx" << idx << std::endl;
        }
        finishTime = std::chrono::high_resolution_clock::now();
        auto ExperimentExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
        //std::cout << "ExperimentExecutionTime: " << ExperimentExecutionTime << " operationsList.size:  " << operationsList.size() << std::endl;
        //std::cout << " mythr:"<<(double)operationsList.size()/ (double)ExperimentExecutionTime * 1000000<<" opr/sec" <<std::endl;
        return (double)operationsList.size()/ (double)ExperimentExecutionTime; // Throughput
    }


/*
  double runBatchThroughput() override{

        assert(valuesList.size() > 0);
        assert(valuesList.size() == keysList.size());
        assert(valuesList.size() == operationsList.size());
        uint64_t idx = 0;
        uint64_t  valSize = valuesList[0].size();
        uint64_t  keySize = (keysList[0]).length();
        uint64_t  loopSize = keysList.size();
        responseTimes.resize(operationsList.size());
        int status = 0;
        startTime = std::chrono::high_resolution_clock::now();
        while (idx < loopSize ) {
            switch (operationsList[idx]) {
                case Operation::putOpr: {
                    //startTime2 = std::chrono::high_resolution_clock::now();
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    Dbt dval(const_cast<char*>(valuesList[idx].c_str()), valSize + 1);
                    status = pdb->put(NULL, &dkey, &dval, 0);
                    assert(status == 0);
                    //finishTime2 = std::chrono::high_resolution_clock::now();
                    //auto Oprduration = std::chrono::duration_cast<std::chrono::microseconds>( finishTime2 - startTime2 ).count();
                    //std::cout << "put duration: : " << Oprduration << std::endl;
                    break;
                }
                case Operation::getOpr: {
                    if(!mergeExist) {
                        char buffer[valSize + 1];
                        Dbt dkey(const_cast<char *>(keysList[idx].c_str()), keySize + 1);
                        Dbt dval;
                        dval.set_data(buffer);
                        dval.set_ulen(valSize + 1);
                        dval.set_flags(DB_DBT_USERMEM);
                        status = pdb->get(NULL, &dkey, &dval, 0);
                        assert(status == 0);
                    } else {
                        //startTime2 = std::chrono::high_resolution_clock::now();
                        try {
                            // database open omitted for clarity

                            // Get a cursor
                            Dbc *cursorp;
                            pdb->cursor(NULL, &cursorp, 0);

                            // Set up our DBTs
                            //Dbt key(search_key, strlen(search_key) + 1);
                            Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                            Dbt data;

                            // Position the cursor to the first record in the database whose
                            // key and data begin with the correct strings.
                            int ret = cursorp->get(&dkey, &data, DB_SET);
                            while (ret != DB_NOTFOUND) {
                                //std::cout << "key: " << (char *)dkey.get_data()
                                //          << "data: " << (char *)data.get_data()<< std::endl;
                                ret = cursorp->get(&dkey, &data, DB_NEXT_DUP);
                            }
                        } catch(DbException &e) {
                            pdb->err(e.get_errno(), "Error!");
                        } catch(std::exception &e) {
                            pdb->errx("Error! %s", e.what());
                        }

                        //finishTime2 = std::chrono::high_resolution_clock::now();
                        //auto Oprduration = std::chrono::duration_cast<std::chrono::microseconds>( finishTime2 - startTime2 ).count();
                        //std::cout << "get duration: : " << Oprduration << std::endl;
                    }
                    break;
                }

                case Operation::mergeOpr:
                {
                    //startTime2 = std::chrono::high_resolution_clock::now();
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    Dbt dval(const_cast<char*>(valuesList[idx].c_str()), valSize + 1);
                    status = pdb->put(NULL, &dkey, &dval, 0);
                    assert(status == 0);
                    //finishTime2 = std::chrono::high_resolution_clock::now();
                    //auto Oprduration = std::chrono::duration_cast<std::chrono::microseconds>( finishTime2 - startTime2 ).count();
                    //std::cout << "merge duration: : " << Oprduration << std::endl;
                    break;

                }
                case Operation::deleteOpr: {
                    //startTime2 = std::chrono::high_resolution_clock::now();
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    status = pdb->del(NULL, &dkey, 0);
                    assert(status == 0);
                    //finishTime2 = std::chrono::high_resolution_clock::now();
                    //auto Oprduration = std::chrono::duration_cast<std::chrono::microseconds>( finishTime2 - startTime2 ).count();
                    //std::cout << "delete  duration: : " << Oprduration << std::endl;
                    break;

                }
            }

            idx ++;
            //if(idx % 10000 == 0)
             //   std::cout << "idx" << idx << std::endl;
        }
        finishTime = std::chrono::high_resolution_clock::now();
        auto ExperimentExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
        //std::cout << "ExperimentExecutionTime: " << ExperimentExecutionTime << " operationsList.size:  " << operationsList.size() << std::endl;
        //std::cout << " mythr:"<<(double)operationsList.size()/ (double)ExperimentExecutionTime * 1000000<<" opr/sec" <<std::endl;
        return (double)operationsList.size()/ (double)ExperimentExecutionTime; // Throughput
    }
    */


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


    void unitTest() {

        std::string key = "hi1";
        std::string value = " totaly differnet yhing";
        uint64_t  keySize = key.length();
        std::cout << " key length: " <<  keySize << std::endl;
        uint64_t  valSize = value.length();
        std::cout << " value lengt: " << valSize  << std::endl;


        // get
        {
            char  buffer[valSize+1];
            Dbt dkey(const_cast<char*>(key.c_str()), keySize+1);
            Dbt dval;
            dval.set_data(buffer);
            dval.set_ulen(valSize + 1);
            dval.set_flags(DB_DBT_USERMEM);
            int rstatus = pdb->get(NULL, &dkey, &dval, 0);
            assert(rstatus != 0);

        }
        //  put
        {



            Dbt dkey(const_cast<char*>(key.c_str()), keySize+1);
            Dbt dval(const_cast<char*>(value.c_str()), valSize+1);
            int rstatus =  pdb->put(NULL, &dkey, &dval, 0);
            assert(rstatus == 0);
            std::cout << " I am here2 "  << std::endl;
        }


        // get

        {
            char buffer[valSize + 1];
            Dbt dkey(const_cast<char*>(key.c_str()), keySize + 1);
            Dbt dval;
            dval.set_data(buffer);
            dval.set_ulen(valSize + 1);
            dval.set_flags(DB_DBT_USERMEM);
            int rstatus = pdb->get(NULL, &dkey, &dval, 0);
            for (int i = 0; i < valSize ; i++) {
                std::cout <<buffer[i] ;
            }
            std::cout << "- "<< std::endl;

            assert(rstatus == 0);
            std::cout << " I am here3 "  << std::endl;
        }

        // put

        {
            value = " hello how are you hgfhgfhgfh sdhfjsdgf shdgfjhsdgfjhsd  hsdgfjhsd ";
            valSize = value.length();
            std::cout<< "new value length" << value.length() << std::endl;
            Dbt dkey(const_cast<char*>(key.c_str()), keySize + 1);
            Dbt dval(const_cast<char*>(value.c_str()), valSize + 1);
            int rstatus = pdb->put(NULL, &dkey, &dval, 0);
            assert(rstatus == 0);
        }


        // get

        {
            // in-place update for bereckley  with  bigger value works
           /*char  buffer [valSize];
             Dbt dkey(const_cast<char*>(key.c_str()), keySize + 1);
             Dbt dval;
             dval.set_data(buffer);
             dval.set_ulen(valSize+ 1);
             dval.set_flags(DB_DBT_USERMEM);
             int rstatus = pdb->get(NULL, &dkey, &dval, 0);
             std::cout << "dval size" << dval.get_size() << std::endl;
             assert(rstatus == 0);
             for (int i = 0; i < valSize ; i++) {
                 std::cout <<buffer[i] ;
             }
             std::cout << "*****"<< std::endl;

             assert(rstatus == 0);
             std::cout << " I am here4 "  << std::endl; */

           /*

            char  buffer [valSize];
            Dbt dkey(const_cast<char*>(key.c_str()), keySize + 1);
            Dbt dval;

            int rstatus = pdb->get(NULL, &dkey, &dval, 0);
            std::cout << "dval size" << dval.get_size() << std::endl;


            assert(rstatus == 0);
            std::cout << "*****"<< std::endl;
            std::cout <<  "data: " << (char *)dval.get_data()<< std::endl;

            assert(rstatus == 0);
            std::cout << " I am here4 "  << std::endl; */

           /* try {
                // database open omitted for clarity

                // Get a cursor
                Dbc *cursorp;
                pdb->cursor(NULL, &cursorp, 0);

                // Set up our DBTs
                //Dbt key(search_key, strlen(search_key) + 1);
                Dbt dkey(const_cast<char*>(key.c_str()), keySize + 1);
                Dbt data;

                // Position the cursor to the first record in the database whose
                // key and data begin with the correct strings.
                int ret = cursorp->get(&dkey, &data, DB_SET);
                while (ret != DB_NOTFOUND) {
                    std::cout << "key: " << (char *)dkey.get_data()
                              << "data: " << (char *)data.get_data()<< std::endl;
                    ret = cursorp->get(&dkey, &data, DB_NEXT_DUP);
                }
            } catch(DbException &e) {
                pdb->err(e.get_errno(), "Error!");
            } catch(std::exception &e) {
                pdb->errx("Error! %s", e.what());
            }
           */

           // rmw with get-put


            std::size_t myValSize = 256;
            key = "new key 1";
            keySize = key.length();
            int rstatus = 0;
            uint32_t  oldValueSize = 0;
            /*
            startTime = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 10000; i++ ) {
                Dbt dkey(const_cast<char*>(key.c_str()), keySize);
                Dbt dval;

                 rstatus = pdb->get(NULL, &dkey, &dval, 0);
                if (rstatus == 0) {// successful
                    //std::cout << "make the other value" << std::endl;
                    // raed it
                    oldValueSize = dval.get_size();
                    //std::cout << "dval size" << oldValueSize << std::endl;
                    //std::cout <<  " dval data: " << (char *)dval.get_data()<< std::endl;

                    // append a new value
                    char buffer[oldValueSize + myValSize];
                    std::memcpy(buffer, dval.get_data(), oldValueSize );
                    std::memset(buffer + oldValueSize,'1', myValSize);

                    // write the new value
                    Dbt dval(buffer, oldValueSize +  myValSize);
                    rstatus =  pdb->put(NULL, &dkey, &dval, 0);
                    assert(rstatus == 0);
                    //std::cout << " --------------- "  << std::endl;

                } else {
                    //std::cout << "make the firts value" << std::endl;
                    Dbt dkey(const_cast<char*>(key.c_str()), keySize);
                    char buffer[myValSize];
                    std::memset(buffer, '0', myValSize);
                    Dbt dval(buffer, myValSize);
                     rstatus =  pdb->put(NULL, &dkey, &dval, 0);
                    assert(rstatus == 0);
                    //std::cout << "---------------"  << std::endl;
                }

            }
            std::cout << "oldValueSize" << oldValueSize << std::endl;
            finishTime = std::chrono::high_resolution_clock::now();
            auto ExperimentExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
            std::cout << "ExperimentExecutionTime: " << ExperimentExecutionTime << std::endl;
           */



            key = "new key 2";
            keySize = key.length();
            auto startTime = std::chrono::high_resolution_clock::now();
            char buffer[myValSize];
            std::memset(buffer, '0', myValSize);
            for (int i = 0; i < 10000; i++ ) {
                    Dbt dkey(const_cast<char*>(key.c_str()), keySize);
                    Dbt dval(buffer, myValSize);
                    rstatus =  pdb->put(NULL, &dkey, &dval, 0);
                    assert(rstatus == 0);
                    //std::cout << "---------------"  << std::endl;
                }

            try {
                // database open omitted for clarity

                // Get a cursor
                Dbc *cursorp;
                pdb->cursor(NULL, &cursorp, 0);

                // Set up our DBTs
                //Dbt key(search_key, strlen(search_key) + 1);
                Dbt dkey(const_cast<char*>(key.c_str()), keySize);
                Dbt data;

                // Position the cursor to the first record in the database whose
                // key and data begin with the correct strings.
                int ret = cursorp->get(&dkey, &data, DB_SET);
                int i = 0;
                uint32_t  oldValueSize;
                while (ret != DB_NOTFOUND) {
                    i++;
                    //std::cout << "key: " << (char *)dkey.get_data()
                    //          << "data: " << (char *)data.get_data()<< std::endl;
                    oldValueSize = data.get_size();
                    ret = cursorp->get(&dkey, &data, DB_NEXT_DUP);


                }
                std::cout << "%%%%%%%%%%%%%%%% i : " << i << "datt size "   << oldValueSize <<  std::endl;
            } catch(DbException &e) {
                pdb->err(e.get_errno(), "Error!");
            } catch(std::exception &e) {
                pdb->errx("Error! %s", e.what());
            }


            auto finishTime = std::chrono::high_resolution_clock::now();
            auto ExperimentExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
            std::cout << "ExperimentExecutionTime2: " << ExperimentExecutionTime << std::endl;


        }




        // delete
        {
            Dbt dkey(const_cast<char*>(key.c_str()), keySize) ;
            int dstatus = pdb->del(NULL, &dkey, 0);
            assert(dstatus == 0);
        }
        // get
        {
            char buffer[valSize + 1];
            Dbt dkey(const_cast<char*>(key.c_str()), keySize + 1);
            Dbt dval;
            dval.set_data(buffer);
            dval.set_ulen(valSize + 1);
            dval.set_flags(DB_DBT_USERMEM);
            int rstatus = pdb->get(NULL, &dkey, &dval, 0);
            assert(rstatus != 0);

        }

        // get

        //  rmw

        // get

        // rmw

        //get

        // rmw

        // rmw

        // rmw

        // get
        // del
        // get
        pdb->close(0);
        delete pdb;
        // You have to close and delete an existing handle, then create a new one before you can use it to remove a database (file).
        pdb = new Db(NULL, 0);
        pdb->remove(file_name, NULL, 0);

        assert(1 == 2);

    }

    void  dumpOperationsOnFile() {}
private:
    std::string connectionString_;
    std::string kDBPath;
    TimeVar startTime, finishTime;
   // TimeVar startTime2, finishTime2;
    std::string response;
    Db* pdb;
    const char *file_name;
    bool dupIsEnabled;

};

#endif //GADGET_BEREKLY_H