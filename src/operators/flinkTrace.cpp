
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "gadget/operator/operators/flinkReplayer.h"

FlinkRePlayer::FlinkRePlayer(const std::string &traceAddress) {
    traceAddress_ = traceAddress;
    // open the file
    FlinkTraceFile_.open(traceAddress_);
    if(!FlinkTraceFile_.is_open()) {
        perror("Flink trace file does  not exist");
        exit(EXIT_FAILURE);
    }
}



bool FlinkRePlayer::runOperator(std::vector<std::shared_ptr<Operation>> &operationList) {
    int i = 0;
    operationList.clear();

    // get the value from here
    uint64_t  valueSize = 256; // fixme
    std::string value(valueSize, '0');

    while (i <  1000) {
        i++;

        if (std::getline(FlinkTraceFile_, currentLine_)) {
            currentLineFields_.clear();
            std::stringstream lineStream(currentLine_);
            std::string field;
            while (lineStream >> field) {
                currentLineFields_.push_back(field);
            }
            // make the operation
            auto opr =  std::make_shared<Operation>();
            if (currentLineFields_[1] == "get") {
                opr->oprType = Operation::getOpr;
            } else  if (currentLineFields_[1] == "put") {
                opr->oprType = Operation::putOpr;
            } else if (currentLineFields_[1] == "merge") {
                opr->oprType = Operation::mergeOpr;
            } else if (currentLineFields_[1] == "delete") {
                opr->oprType = Operation::deleteOpr;
            } else if (currentLineFields_[1] == "scan") {
                opr->oprType = Operation::scanOpr;
            } else if (currentLineFields_[1] == "next") {
                opr->oprType = Operation::nextOpr;
            }
            opr->key = currentLineFields_[2];
            //opr->value = currentLineFields_[3];
            opr->value = value; // fixme
            operationList.push_back(opr);
        } else {
            break;
        }
    }

    if(operationList.size() > 0) {
        return true;
    } else {
        return false;
    }

}



