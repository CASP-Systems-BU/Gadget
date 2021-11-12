//
// Created by Showan Asyabi on 3/17/21.
//

#ifndef GADGET_SLIDINGKEYEDINCREMENTAL_H
#define GADGET_SLIDINGKEYEDINCREMENTAL_H
#include "include/gadget/operator/oneStreamWindowOperator.h"

class SlidingKeyedIncremental :   public OneStreamWindowOperator {
public:
    explicit SlidingKeyedIncremental (std::shared_ptr<OperatorParameters> params);
    void  assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) override;
};



#endif //GADGET_SLIDINGKEYEDINCREMENTAL_H
