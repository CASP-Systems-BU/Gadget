//
// Created by Showan Asyabi on 3/11/21.
//
#ifndef GADGET_SLIDINGALLINCREMENTAL_H
#define GADGET_SLIDINGALLINCREMENTAL_H

#include "include/gadget/operator/oneStreamWindowOperator.h"

class SlidingAllIncremental :   public OneStreamWindowOperator {
public:
    explicit SlidingAllIncremental (std::shared_ptr<OperatorParameters> params);
    void  assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) override;
};




#endif //GADGET_SLIDINGALLINCREMENTAL_H
