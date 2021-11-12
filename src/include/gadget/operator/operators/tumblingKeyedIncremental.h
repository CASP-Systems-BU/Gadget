//
// Created by Showan Asyabi on 3/23/21.
//

#ifndef GADGET_TUMBLINGKEYEDINCREMENTAL_H
#define GADGET_TUMBLINGKEYEDINCREMENTAL_H

#include "include/gadget/operator/oneStreamWindowOperator.h"

class TumblingKeyedIncremental :   public OneStreamWindowOperator {
public:
    explicit TumblingKeyedIncremental (std::shared_ptr<OperatorParameters> params);
    void  assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) override;
};




#endif //GADGET_TUMBLINGKEYEDINCREMENTAL_H
