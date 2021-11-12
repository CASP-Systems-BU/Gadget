//
// Created by Showan Asyabi on 1/25/21.
//

#ifndef GADGET_OPERATION_H
#define GADGET_OPERATION_H

#include <memory>
#include <string>
#include <vector>

class Operation {
public:
    enum OperationType :uint8_t {
        getOpr = 0, putOpr, mergeOpr ,deleteOpr,scanOpr, nextOpr
    };

    Operation() {
        responseTime = 0;
    }

    bool operator< (const Operation &other) const {
        return responseTime < other.responseTime;
    }

public:
    int64_t responseTime;
    OperationType oprType;
    std::string key;
    std::string value;
    uint64_t  sleepTime;
};

#endif //GADGET_WTUMBLING_H
