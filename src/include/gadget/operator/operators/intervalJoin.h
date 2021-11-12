//
// Created by Showan Asyabi on 3/18/21.
//
#ifndef GADGET_INTERVALJOIN_H
#define GADGET_INTERVALJOIN_H
#include "gadget/operator/twoStreamContinuousOperator.h"

class IntervalJoin : public TwoStreamContinuousOperator  {
public:
    explicit IntervalJoin(std::shared_ptr<OperatorParameters> params);
};

#endif //GADGET_INTERVALJOIN_H
