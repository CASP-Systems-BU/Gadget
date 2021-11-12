//
// Created by Showan Asyabi on 3/23/21.
//

#ifndef GADGET_JOINSLIDING_H
#define GADGET_JOINSLIDING_H

#include "include/gadget/operator/twoStreamWindowOperator.h"
class JoinSliding :   public TwoStreamWindowOperator {
public:
    explicit JoinSliding (std::shared_ptr<OperatorParameters> params);
    void  assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) override;
};

#endif //GADGET_JOINSLIDING_H
