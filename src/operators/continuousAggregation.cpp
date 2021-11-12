//
// Created by Showan Asyabi on 3/23/21.
//

#include "include/gadget/operator/operators/continuousAggregation.h"

ContinuousAggregation::ContinuousAggregation (std::shared_ptr<OperatorParameters> params){
        keyedOperator_ = true;
        valueDistribution_ = params->valuesDistribution;
        eventGenerator_ = params->eventGenerator;
        serviceTimeDistribution_ = params->serviceTimeDistribution;
        keyDistribution_ = params->keysDistribution;
        stateMachineType_ = StateMachines::contAggr;
    }
