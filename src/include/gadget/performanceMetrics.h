//
// Created by Showan Asyabi on 1/25/21.
//

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "gadget/kvwrappers/kvwrapper.h"

#ifndef GADGET_PERFORMANCEMETRICS_H
#define GADGET_PERFORMANCEMETRICS_H
#include "gadget/operation.h"
class PerformanceMetrics {

public:
    PerformanceMetrics(std::shared_ptr<KVWrapper> KV): KVstore(KV), sorted_(false), isBenchmarkDone(false){
        KVstore = KV;
    }

     double  ExpP95();
     double  ExpP99();
     double ExpP99_9();
     double ExpP99_99();
     double ExpP50();
     double ExpAvg();
     int64_t ExpMax();
     int64_t ExpMin();
     double ExpSd();

public:
    bool sorted_;
    bool isBenchmarkDone;
    std::shared_ptr<KVWrapper> KVstore;
};
#endif //GADGET_PERFORMANCEMETRICS_H
