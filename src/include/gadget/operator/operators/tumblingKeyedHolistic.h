//
// Created by Showan Asyabi on 3/23/21.
//

#ifndef GADGET_TUMBLINGKEYEDHOLISTIC_H
#define GADGET_TUMBLINGKEYEDHOLISTIC_H
#include "include/gadget/operator/oneStreamWindowOperator.h"

class TumblingKeyedHolistic :   public OneStreamWindowOperator {
public:
    explicit TumblingKeyedHolistic (std::shared_ptr<OperatorParameters> params);
    void  assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) override;
};

#endif //GADGET_TUMBLINGKEYEDHOLISTIC_H
