//
// Created by Showan Asyabi on 3/22/21.
//

#ifndef GADGET_SLIDINGALLHOLISTIC_H
#define GADGET_SLIDINGALLHOLISTIC_H

#include "include/gadget/operator/oneStreamWindowOperator.h"
class SlidingAllHolistic :   public OneStreamWindowOperator {
public:
    explicit SlidingAllHolistic (std::shared_ptr<OperatorParameters> params);
    void  assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) override;
};


#endif //GADGET_SLIDINGALLHOLISTIC_H
