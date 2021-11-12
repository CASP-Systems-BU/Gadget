//
// Created by Showan Asyabi on 1/22/21.
//

#ifndef GADGET_KVWRAPPER_H
#define GADGET_KVWRAPPER_H
#include "include/gadget/operation.h"


class KVWrapper {
public:
    int64_t  responseTime;
    enum  KVType : uint16_t {
        RocksDB = 0, Faster, Lethe, File, BerkeleyDB
    };
    virtual ~KVWrapper() = default;

    virtual bool connect() = 0;
    virtual bool disconnect() = 0;


    /**
     *  run a a batch of commands, record the type and the latency of individual requests
     */
    virtual void runBatchLatency() = 0 ;


    /**
    *  run a a batch of commands, have a timer ate the beginning  and  at the end of the experiment to quantify the
     *  throughput more closely
    */
    virtual double runBatchThroughput() = 0 ;


    /**
     *  dump the generated  operations on  aFile
     * @return
     */
    virtual void dumpOperationsOnFile() = 0;

    bool mergeExist; // a workaround for bereklydb
    std::vector<std::string>  valuesList;
    std::vector<std::string>  keysList;
    std::vector<Operation::OperationType>  operationsList;
    std::vector<double>  sleepTimes;
    std::vector<int64_t> responseTimes;
};
#endif //GADGET_KVWRAPPER_H
