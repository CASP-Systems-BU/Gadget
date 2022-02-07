//
// Created by Showan Asyabi on 4/3/21.
//
// copy the content of the berecklyDB.temp to this file when you do experiments

#ifndef GADGET_BerecklyDBWRAPPER_H
#define GADGET_BerecklyDBWRAPPER_H

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "include/gadget/performanceMetrics.h"
#include "include/gadget/kvwrappers/kvwrapper.h"
typedef std::chrono::high_resolution_clock::time_point TimeVar;


class BerkeleyDBWrapper: public KVWrapper {
public:
    explicit BerkeleyDBWrapper(const std::string& connectionString) {
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


#endif //GADGET_BerecklyDBWRAPPER_H
