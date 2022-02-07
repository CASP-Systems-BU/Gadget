//
// Created by Showan Asyabi on 5/23/21.
//

#ifndef GADGET_OPERATOR_H
#define GADGET_OPERATOR_H
#include "include/gadget/operation.h"
class  Operator {
public:
    enum  operatorName : uint16_t {
        none = 0 , OperatorWindowTumblingAllIncremental,
        OperatorWindowTumblingAllHolistic,
        OperatorWindowTumblingKeyedIncremental,
        OperatorWindowTumblingKeyedHolistic ,
        OperatorWindowSlidingAllIncremental,
        OperatorWindowSlidingAllHolistic,
        OperatorWindowSlidingKeyedIncremental,
        OperatorWindowSlidingKeyedHolistic,
        OperatorJoinTumbling,
        OperatorJoinSliding,
        OperatorJoinInterval ,
        OperatorContinuousAggregation,
        OperatorFlinkTrace,
        OperatorYCSBTrace,
        OperatorContinuousJoin,
        OperatorSessionHolistic,
        OperatorSessionIncremental
    };
    virtual ~Operator() = default;
    virtual  bool runOperator(std::vector<std::shared_ptr<Operation>> &operationList)  = 0;
};

#endif //GADGET_OPERATOR_H
