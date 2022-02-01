//
// Created by Showan Asyabi on 1/21/22.
//

#ifndef GADGET_SESSIONHOLISTIC_H
#define GADGET_SESSIONHOLISTIC_H

#include "include/gadget/operator/oneStreamSessionWindowOperator.h"
class SessionHolistic :   public OneStreamSessionWindowOperator {
public:
    explicit SessionHolistic (std::shared_ptr<OperatorParameters> params);
    void  assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) override;
};


#endif //GADGET_SESSIONHOLISTIC_H
