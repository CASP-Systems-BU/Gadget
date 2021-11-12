//
// Created by Showan Asyabi on 3/18/21.
//

#ifndef GADGET_JINTUMBLING_H
#define GADGET_JINTUMBLING_H
#include "include/gadget/operator/twoStreamWindowOperator.h"
class JoinTumbling :   public TwoStreamWindowOperator {
public:
    explicit JoinTumbling (std::shared_ptr<OperatorParameters> params);
    void  assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) override;
};






#endif //GADGET_JINTUMBLING_H
