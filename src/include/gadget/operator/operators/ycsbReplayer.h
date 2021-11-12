//
// Created by Showan Asyabi on 6/29/21.
//

#ifndef GADGET_YCSBREPLAYER_H
#define GADGET_YCSBREPLAYER_H

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "gadget/operator/operator.h"

class YCSBRePlayer : public Operator {
public:
    explicit YCSBRePlayer (const std::string &traceAddress);
    bool runOperator(std::vector<std::shared_ptr<Operation>> &operationList);
    
private:
    std::string traceAddress_;
    std::string currentLine_;
    std::vector<std::string> currentLineFields_;
    std::ifstream ycsbTraceFile_;
};
#endif //GADGET_YCSBREPLAYER_H
