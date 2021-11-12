//
// Created by Showan Asyabi on 3/17/21.
//

#include "gadget/operator/operators/joinTumbling.h"

JoinTumbling::JoinTumbling (std::shared_ptr<OperatorParameters> params){
    keyedOperator_ = true;
    windowLength_ = params->windowLength;
    latenessAllowed_ = params->latenessLength;
    latenessLength_ = params->latenessLength;
    keyDistribution_ = params->keysDistribution;
    valueDistribution_ = params->valuesDistribution;
    eventGenerator_ = params->eventGenerator;
    eventGenerator2_ = params->eventGenerator2;
    serviceTimeDistribution_ = params->serviceTimeDistribution;
    stateMachineType_ = StateMachines::holistic;

}

void  JoinTumbling::assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) {
        uint64_t  windowStartTime = std::floor( event->eventTime_ /windowLength_) * windowLength_;
        windowStartTimes.push_back(windowStartTime);
    }