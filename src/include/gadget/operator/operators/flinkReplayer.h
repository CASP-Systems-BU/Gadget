//
// Created by Showan Asyabi on 6/29/21.
//

#ifndef GADGET_FLINKREPLAYER_H
#define GADGET_FLINKREPLAYER_H
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <map>
#include <unordered_map>

#include "include/gadget/operator/operator.h"
class FlinkRePlayer : public Operator {
public:
    explicit FlinkRePlayer (const std::string &traceAddress);
    bool runOperator(std::vector<std::shared_ptr<Operation>> &operationList);


private:
    std::string traceAddress_;
    std::string currentLine_;
    std::vector<std::string> currentLineFields_;
    std::ifstream FlinkTraceFile_;
};


#endif //GADGET_FLINKREPLAYER_H
