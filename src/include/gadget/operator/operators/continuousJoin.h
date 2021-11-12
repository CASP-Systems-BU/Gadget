//
// Created by Showan Asyabi on 5/24/21.
//

#ifndef GADGET_CONTINUOUSJOIN_H
#define GADGET_CONTINUOUSJOIN_H

#include "gadget/operator/twoStreamContinuousOperator.h"

class ContinuousJoin : public TwoStreamContinuousOperator  {
public:
    explicit ContinuousJoin (std::shared_ptr<OperatorParameters> params);
};


#endif //GADGET_CONTINUOUSJOIN_H
