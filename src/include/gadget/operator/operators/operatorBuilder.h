//
// Created by Showan Asyabi on 5/24/21.
//

#ifndef GADGET_OPERATORBUILDER_H
#define GADGET_OPERATORBUILDER_H


#include "include/gadget/operator/operators/slidingAllIncremental.h"
#include "include/gadget/operator/operators/slidingKeyedIncremental.h"
#include "include/gadget/operator/operators/slidingKeyedHolistic.h"
#include "include/gadget/operator/operators/joinTumbling.h"
#include "include/gadget/operator/operators/operatorParameters.h"
#include "include/gadget/operator/operators/slidingAllHolistic.h"
#include "include/gadget/operator/operators/tumblingAllIncremental.h"
#include "include/gadget/operator/operators/tumblingAllHolistic.h"
#include "include/gadget/operator/operators/tumblingKeyedIncremental.h"
#include "include/gadget/operator/operators/tumblingKeyedHolistic.h"
#include "include/gadget/operator/operators/joinSliding.h"
#include "include/gadget/operator/operators/intervalJoin.h"
#include "include/gadget/operator/operators/continuousAggregation.h"
#include "include/gadget/operator/operators/continuousJoin.h"
#include "include/gadget/operator/operators/ycsbReplayer.h"
#include "include/gadget/operator/operators/flinkReplayer.h"




class OperatorBuilder {
public:
    static std::optional<std::shared_ptr<Operator>> BuildOperator(std::shared_ptr<OperatorParameters> params) {
        if (params->operatorType == Operator::OperatorWindowTumblingAllIncremental) {
            return std::make_shared<TumblingAllIncremental>(params);
        } if(params->operatorType == Operator::OperatorWindowTumblingAllHolistic) {
            return std::make_shared<TumblingAllHolistic>(params);
        } if(params->operatorType == Operator::OperatorWindowTumblingKeyedIncremental) {
            return std::make_shared<TumblingKeyedIncremental>(params);
        } if (params->operatorType == Operator::OperatorWindowTumblingKeyedHolistic) {
            return std::make_shared<TumblingKeyedHolistic>(params);
        }  else if(params->operatorType == Operator::OperatorWindowSlidingAllIncremental) {
            return std::make_shared<SlidingAllIncremental>(params);
        } else if(params->operatorType == Operator::OperatorWindowSlidingKeyedHolistic) {
            return std::make_shared<SlidingKeyedHolistic>(params);
        } else if(params->operatorType == Operator::OperatorWindowSlidingAllHolistic) {
            return std::make_shared<SlidingAllHolistic>(params);
        } else if(params->operatorType == Operator::OperatorWindowSlidingKeyedIncremental) {
            return std::make_shared<SlidingKeyedIncremental>(params);
        } else if(params->operatorType == Operator::OperatorContinuousAggregation) {
            return std::make_shared<ContinuousAggregation>(params);
        }  else if(params->operatorType == Operator::OperatorJoinTumbling) {
            return std::make_shared<JoinTumbling>(params);
        } else if(params->operatorType == Operator::OperatorJoinSliding) {
            return std::make_shared<JoinSliding>(params);
        }  else if(params->operatorType == Operator::OperatorContinuousJoin) {
            return std::make_shared<ContinuousJoin>(params);
        }   else if(params->operatorType == Operator::OperatorJoinInterval) {
            return std::make_shared<IntervalJoin>(params);
        } else if (params->operatorType == Operator::OperatorFlinkTrace) {
            return std::make_shared<FlinkRePlayer>(params->traceFilePath);
        } else if (params->operatorType == Operator::OperatorYCSBTrace) {
            return std::make_shared<YCSBRePlayer>(params->traceFilePath);
        }
        return {};
    }

};

#endif //GADGET_OPERATORBUILDER_H
