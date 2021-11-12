
#include "include/gadget/operator/operators/continuousJoin.h"

ContinuousJoin::ContinuousJoin (std::shared_ptr<OperatorParameters> params){
    keyedOperator_ = true;
    valueDistribution_ = params->valuesDistribution;
    eventGenerator_ = params->eventGenerator;
    eventGenerator2_  = params->eventGenerator2;
    serviceTimeDistribution_ = params->serviceTimeDistribution;
    keyDistribution_ = params->keysDistribution;
    stateMachineType_ = StateMachines::contJoin;
}



