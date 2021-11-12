
#include <string>
#include "gadget/operator/operators/ycsbReplayer.h"

YCSBRePlayer::YCSBRePlayer(const std::string &traceAddress) {
    traceAddress_ = traceAddress;
    // open the file
    ycsbTraceFile_.open(traceAddress_);




    if(!ycsbTraceFile_.is_open()) {
        perror("YCSB trace file does  not exist");
        exit(EXIT_FAILURE);
    }
}



bool YCSBRePlayer::runOperator(std::vector<std::shared_ptr<Operation>> &operationList) {
    int i = 0;
    operationList.clear();

    // get the value from here
    uint64_t  valueSize = 256; // fixme
    std::string value(valueSize, '0');

    operationList.clear();
    while (i <  1000) {  // in each call get 1000 operations
        i++;

        if (std::getline(ycsbTraceFile_, currentLine_)) {
            currentLineFields_.clear();

            std::stringstream lineStream(currentLine_);
            std::string field;
            while (lineStream >> field) {
                currentLineFields_.push_back(field);
            }
            // make the operation
            auto opr =  std::make_shared<Operation>();
            if (currentLineFields_[0] == "READ") {
                opr->oprType = Operation::getOpr;
            } else  if (currentLineFields_[0] == "UPDATE") {  // fixme ask Vasia about this =
                opr->oprType = Operation::mergeOpr;
            } else if (currentLineFields_[0] == "INSERT") {
                opr->oprType = Operation::putOpr;
            } else if (currentLineFields_[0] == "SCAN") {
                opr->oprType = Operation::scanOpr;
            }
            opr->key = currentLineFields_[2];
            //opr->value = currentLineFields_[3];
            opr->value = value;
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



