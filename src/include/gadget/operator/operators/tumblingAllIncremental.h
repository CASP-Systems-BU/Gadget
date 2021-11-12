//
// Created by Showan Asyabi on 3/23/21.
//

#ifndef GADGET_TUMBLINGALLINCREMENTAL_H
#define GADGET_TUMBLINGALLINCREMENTAL_H

#include "include/gadget/operator/oneStreamWindowOperator.h"

class TumblingAllIncremental :   public OneStreamWindowOperator {
public:
    explicit TumblingAllIncremental (std::shared_ptr<OperatorParameters> params);
    void  assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) override;
};

#endif //GADGET_TUMBLINGALLINCREMENTAL_H
