//
// Created by Showan Asyabi on 3/23/21.
//

#ifndef GADGET_TUMBLINGALLHOLISTIC_H
#define GADGET_TUMBLINGALLHOLISTIC_H
#include "include/gadget/operator/oneStreamWindowOperator.h"
class TumblingAllHolistic :   public OneStreamWindowOperator {
public:
    explicit TumblingAllHolistic (std::shared_ptr<OperatorParameters> params);
    void  assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) override;
};
#endif //GADGET_TUMBLINGALLHOLISTIC_H
