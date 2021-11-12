//
// Created by Showan Asyabi on 4/3/21.
//

#ifndef GADGET_FasterWRAPPER_H
#define GADGET_FasterWRAPPER_H

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>


#include "include/gadget/performanceMetrics.h"
#include "include/gadget/kvwrappers/kvwrapper.h"
typedef std::chrono::high_resolution_clock::time_point TimeVar;


class FasterWrapper: public KVWrapper {
public:
    explicit FasterWrapper(const std::string& connectionString) {
    }

    bool connect() override {
        std::cout << " connected to faster" << std::endl;
        return true;
    }

    bool disconnect() override {
        std::cout << " disconnected from faster" << std::endl;
        return true;
    }

    void dumpOperationsOnFile() override {}

    void runBatchLatency() {}
    double runBatchThroughput() {
        // not defined
        return 0; }
};


#endif //GADGET_FasterWRAPPER_H
