//
// Created by Showan Asyabi on 6/6/21.
//

#ifndef GADGET_ROCKSDBWRAPPER_H
#define GADGET_ROCKSDBWRAPPER_H

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "include/gadget/performanceMetrics.h"
#include "include/gadget/kvwrappers/kvwrapper.h"
typedef std::chrono::high_resolution_clock::time_point TimeVar;


class RocksDBWrapper: public KVWrapper {
public:
    explicit RocksDBWrapper(const std::string& connectionString) {
    }

    bool connect() override {
        return true;
    }

    bool disconnect() override {
        return true;
    }

    void dumpOperationsOnFile() override {}

    void runBatchLatency() {}
    double runBatchThroughput() {
        // not defined
        return 0; }

};


#endif
