//
// Created by Showan Asyabi on 3/22/21.
//

#ifndef GADGET_SLIDINGKEYEDHOLISTIC_H
#define GADGET_SLIDINGKEYEDHOLISTIC_H
#include "include/gadget/operator/oneStreamWindowOperator.h"

class SlidingKeyedHolistic :   public OneStreamWindowOperator {
public:
    explicit SlidingKeyedHolistic (std::shared_ptr<OperatorParameters> params);
    void  assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) override;
};

#endif //GADGET_SLIDINGKEYEDHOLISTIC_H
