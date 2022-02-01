//
// Created by Showan Asyabi on 1/21/22.
//

#include "include/gadget/operator/operators/sessionHolistic.h"

SessionHolistic::SessionHolistic(std::shared_ptr<OperatorParameters> params) {
    keyedOperator_ = false;
    windowLength_ = params->windowLength;
    slidingLength_ = params ->slidingLength;
    latenessAllowed_ = params->latenessLength;
    latenessLength_ =  params->latenessLength;
    keyDistribution_ = params->keysDistribution;
    valueDistribution_ = params->valuesDistribution;
    eventGenerator_ = params->eventGenerator;
    serviceTimeDistribution_ = params->serviceTimeDistribution;
    stateMachineType_ =  StateMachines::holistic;

}

void  SessionHolistic::assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) {
    windowStartTimes.push_back(event->eventTime_);
}