//
// Created by Showan Asyabi on 5/23/21.
//

#ifndef GADGET_OPERATORPARAMETERS_H
#define GADGET_OPERATORPARAMETERS_H

#include "gadget/distributions/serviceTime.h"
#include "gadget/distributions/arrival.h"
#include "gadget/distributions/value.h"
#include "gadget/statemachine/stateMachines.h"
#include "gadget/operator/operator.h"
#include "gadget/event/eventGenerator.h"
class  OperatorParameters{
public:
    OperatorParameters() {
        expectedNumOperations = 0;
        windowLength = 0;
        slidingLength = 0;
        allowedLateness = false;
        keyedOperator_ = false;
        latenessLength = 0;
    }
    Operator::operatorName operatorType;
    bool      keyedOperator_;
    uint64_t  expectedNumOperations;
    uint64_t  windowLength;
    uint64_t  slidingLength;
    bool allowedLateness;
    uint64_t latenessLength;
    int64_t  upperBound;
    int64_t  lowerBound;
    std::shared_ptr<EventGenerator> eventGenerator;
    std::shared_ptr<EventGenerator> eventGenerator2;
    std::shared_ptr<Arrival> arrivalsDistribution;
    std::shared_ptr<Key> keysDistribution;
    std::shared_ptr<Value> valuesDistribution;
    std::shared_ptr<ServiceTime> serviceTimeDistribution;
    std::string traceFilePath;
    StateMachines::StateMachineType stateMachineType;
};




#endif //GADGET_OPERATORPARAMETERS_H
