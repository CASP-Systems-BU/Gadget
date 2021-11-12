//
// Created by Showan Asyabi on 4/21/21.
//

#ifndef GADGET_BEREKLY_H
#define GADGET_BEREKLY_H

#include <memory>
#include <string>
#include <vector>

#include "include/gadget/performanceMetrics.h"
#include "include/gadget/kvwrappers/kvwrapper.h"
typedef std::chrono::high_resolution_clock::time_point TimeVar;


#include <db_cxx.h>

#define VALUESIZE 2048




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
        if(mergeExist) {
            pdb->set_flags(DB_DUP);
        }
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


    void runBatchLatency()  override {
        uint64_t idx = 0;
        uint64_t  valSize = VALUESIZE;
        uint64_t  keySize = (keysList[0]).length();
        uint64_t  loopSize =   keysList.size();
        responseTimes.resize(operationsList.size());
        assert(valuesList.size() == keysList.size());
        assert(operationsList.size() == keysList.size());

        while (idx < loopSize) {
            switch (operationsList[idx]) {
                case Operation::putOpr: {
                    //unsigned char xkey[sizeof(keysList[idx])];
                    //std::copy( keysList[idx].begin(), keysList[idx].end(), xkey );
                    startTime = std::chrono::high_resolution_clock::now();
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    Dbt dval(const_cast<char*>(valuesList[idx].c_str()), valSize + 1);
                    int status = pdb->put(NULL, &dkey, &dval, 0);
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
                    char buffer[VALUESIZE + 1];
                    if(!mergeExist) {
                        Dbt dkey(const_cast<char *>(keysList[idx].c_str()), keySize + 1);
                        Dbt dval;
                        dval.set_data(buffer);
                        dval.set_ulen(VALUESIZE + 1);
                        dval.set_flags(DB_DBT_USERMEM);
                        int rstatus = pdb->get(NULL, &dkey, &dval, 0);
                        /*
                        if (rstatus == DB_NOTFOUND) {
                            std::cout << "get - Not found" << std::endl;
                            } else {
                            std::cout << "get Found: " << buffer << std::endl;
                        } */
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
                        /*unsigned char xkey[sizeof(keysList[idx])];
                        std::copy( keysList[idx].begin(), keysList[idx].end(), xkey );
                        startTime = std::chrono::high_resolution_clock::now();
                        Dbt dkey(const_cast<unsigned char*>(xkey), (keysList[idx]).length());
                        Dbt dval_(const_cast<char*>(valuesList[idx].c_str()), valSize);

                        // get the current value
                        char buffer[VALUESIZE];
                        Dbt dval;
                        dval.set_data(buffer);
                        dval. (VALUESIZE);
                        dval.set_flags(DB_DBT_USERMEM);

                        int rstatus = pdb->get(NULL, &dkey, &dval, 0);
                        // apend a new value entry the tail of current value

                        pdb->put(NULL, &dkey, &dval_, 0);*/
                       // pdb->set_flags(DB_DUP);
                       // merge is similar to put; however, we turn on the duplicates
                        startTime = std::chrono::high_resolution_clock::now();
                        Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                        Dbt dval(const_cast<char*>(valuesList[idx].c_str()), valSize + 1);
                        int status =  pdb->put(NULL, &dkey, &dval, 0);
                        finishTime = std::chrono::high_resolution_clock::now();
                        assert(status == 0);
                        break;
                    }
                case Operation::deleteOpr:
                {   //unsigned char xkey[sizeof(keysList[idx])];
                    //uint64_t keySize = keysList[idx].length();
                    //std::copy( keysList[idx].begin(), keysList[idx].end(), xkey );
                    //memcpy(xkey,&keysList[idx],sizeof(keysList[idx]));
                    startTime = std::chrono::high_resolution_clock::now();
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    int dstatus = pdb->del(NULL, &dkey, 0);
                    finishTime = std::chrono::high_resolution_clock::now();
                    assert(dstatus == 0);
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


    double runBatchThroughput() override{

        assert(valuesList.size() > 0);
        assert(valuesList.size() == keysList.size());
        assert(valuesList.size() == operationsList.size());
        uint64_t idx = 0;
        uint64_t  valSize = VALUESIZE; // fixme
        uint64_t  keySize = (keysList[0]).length();
        uint64_t  loopSize = keysList.size();
        responseTimes.resize(operationsList.size());
        startTime = std::chrono::high_resolution_clock::now();
        while (idx < loopSize ) {
            switch (operationsList[idx]) {
                case Operation::putOpr: {
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    Dbt dval(const_cast<char*>(valuesList[idx].c_str()), valSize + 1);
                    pdb->put(NULL, &dkey, &dval, 0);
                    break;
                }
                case Operation::getOpr: {
                    if(!mergeExist) {
                        char buffer[VALUESIZE + 1];
                        Dbt dkey(const_cast<char *>(keysList[idx].c_str()), keySize + 1);
                        Dbt dval;
                        dval.set_data(buffer);
                        dval.set_ulen(VALUESIZE + 1);
                        dval.set_flags(DB_DBT_USERMEM);
                        int rstatus = pdb->get(NULL, &dkey, &dval, 0);
                    } else {
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
                    }
                    break;
                }

                case Operation::mergeOpr:
                {
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    Dbt dval(const_cast<char*>(valuesList[idx].c_str()), valSize + 1);
                    pdb->put(NULL, &dkey, &dval, 0);
                    break;
                }
                case Operation::deleteOpr: {
                    Dbt dkey(const_cast<char*>(keysList[idx].c_str()), keySize + 1);
                    int dstatus = pdb->del(NULL, &dkey, 0);
                    break;
                }
            }

            idx ++;
        }
        finishTime = std::chrono::high_resolution_clock::now();
        auto ExperimentExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>( finishTime - startTime ).count();
        std::cout << "ExperimentExecutionTime: " << ExperimentExecutionTime << " operationsList.size:  " << operationsList.size() << std::endl;
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


    void unitTest() {

        std::string key = "hi1";
        std::string value = " totaly differnet yhing";
        uint64_t  keySize = key.length();
        std::cout << " key length" <<  keySize << std::endl;
        uint64_t  valSize = value.length();
        std::cout << " value length" << valSize  << std::endl;


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
            Dbt dkey(const_cast<char*>(key.c_str()), keySize + 1);
            Dbt dval(const_cast<char*>(value.c_str()), valSize + 1);
            int rstatus = pdb->put(NULL, &dkey, &dval, 0);
            assert(rstatus == 0);
        }

        // get

        {
           /* char  buffer [valSize];
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

            try {
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


        }
        // delete
        {
            Dbt dkey(const_cast<char*>(key.c_str()), keySize + 1) ;
            int dstatus = pdb->del(NULL, &dkey, 0);
            assert(dstatus == 0);
        }
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
    std::string response;
    Db* pdb;
    const char *file_name;
    bool dupIsEnabled;

};

#endif //GADGET_BEREKLY_H