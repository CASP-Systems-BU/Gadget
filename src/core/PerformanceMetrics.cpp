//
// Created by Showan Asyabi on 1/29/21.
//

#include "gadget/performanceMetrics.h"

#if 0 // Fixme - remove
struct CompareOperations
{
    bool const operator()(std::shared_ptr<Operation> first, std::shared_ptr<Operation> second) const
    {
        return (*first) < (*second);
    }
};
#endif

 double PerformanceMetrics::ExpAvg() {
    int64_t  sum = 0;
    int64_t count = 0;
    for(long i = 0; i < KVstore->responseTimes.size(); i++) {
        sum += KVstore->responseTimes[i];
        count += 1;
    }
    return (double )sum/(double)count;
}
int64_t PerformanceMetrics::ExpMax() {
    if(!sorted_) {
        //std::sort(KVstore->responseTimes.begin(), KVstore->responseTimes.end(), CompareOperations());
        std::sort(KVstore->responseTimes.begin(), KVstore->responseTimes.end());
        sorted_ = true;
    }
    return KVstore->responseTimes.back();
}

int64_t PerformanceMetrics::ExpMin() {
    if(!sorted_) {
        std::sort(KVstore->responseTimes.begin(), KVstore->responseTimes.end());
        sorted_ = true;
    }
    return KVstore->responseTimes.front();
}

double PerformanceMetrics::ExpP50() {
    if(!sorted_) {
        std::sort(KVstore->responseTimes.begin(), KVstore->responseTimes.end());
        sorted_ = true;
    }
    size_t n = KVstore->responseTimes.size() / 2;
    return KVstore->responseTimes[n];
}

double PerformanceMetrics::ExpP95() {
    if(!sorted_) {
        std::sort(KVstore->responseTimes.begin(), KVstore->responseTimes.end());
        sorted_ = true;
    }
    size_t n = (KVstore->responseTimes.size() * 95) /100;
    return KVstore->responseTimes[n];
}

double PerformanceMetrics::ExpP99() {
    if(!sorted_) {
        std::sort(KVstore->responseTimes.begin(), KVstore->responseTimes.end());
        sorted_ = true;
    }
    size_t n = (KVstore->responseTimes.size() * 99) /100;
    return KVstore->responseTimes[n];
}

double PerformanceMetrics::ExpP99_9() {
    if(!sorted_) {
        std::sort(KVstore->responseTimes.begin(), KVstore->responseTimes.end());
        sorted_ = true;
    }
    size_t n = (KVstore->responseTimes.size() * 999) /1000;
    return KVstore->responseTimes[n];
}

double PerformanceMetrics::ExpP99_99() {
    if(!sorted_) {
        std::sort(KVstore->responseTimes.begin(), KVstore->responseTimes.end());
        sorted_ = true;
    }
    size_t n = (KVstore->responseTimes.size() * 9999) /10000;
    return KVstore->responseTimes[n];
}