//
// Created by Showan Asyabi on 3/23/21.
//
#ifndef GADGET_CONTINUOUSAGGREGATION_H
#define GADGET_CONTINUOUSAGGREGATION_H
#include "gadget/operator/oneStreamContinuousOperator.h"
class ContinuousAggregation :   public  OneStreamContinuousOperator {
public:
    explicit ContinuousAggregation (std::shared_ptr<OperatorParameters> params);
};

#endif //GADGET_CONTINUOUSAGGREGATION_H