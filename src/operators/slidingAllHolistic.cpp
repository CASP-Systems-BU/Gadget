//
// Created by Showan Asyabi on 3/22/21.
//


#include "include/gadget/operator/operators/slidingAllHolistic.h"

  SlidingAllHolistic::SlidingAllHolistic (std::shared_ptr<OperatorParameters> params){
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

    void  SlidingAllHolistic::assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) {
        uint64_t lastWindowStartTime =  std::floor(event->eventTime_/slidingLength_) * slidingLength_;
        while (event->eventTime_ >= lastWindowStartTime && event->eventTime_ <  lastWindowStartTime  + windowLength_) {
            windowStartTimes.push_back(lastWindowStartTime);
            lastWindowStartTime -= slidingLength_;
        }
    }