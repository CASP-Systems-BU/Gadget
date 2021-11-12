//
// Created by Showan Asyabi on 5/20/21.
//

#ifndef GADGET_STATEMACHINEBUILDER_H
#define GADGET_STATEMACHINEBUILDER_H

#include "include/gadget/statemachine/stateMachines.h"
#include "include/gadget/statemachine/windowHolisticSM.h"
#include "include/gadget/statemachine/windowIncrementalSM.h"
#include "include/gadget/statemachine/conJoinSM.h"
#include "include/gadget/statemachine/intervalJoinSM.h"
#include "include/gadget/statemachine/contAggrSM.h"

class StateMachineBuilder {
public:
    static std::optional<std::shared_ptr<StateMachines>> BuildStateMachine(StateMachines::StateMachineType type, std::string machineKey,  uint64_t startTime,
                                                                 uint64_t finishTime) {
        if (type == StateMachines::holistic) {
           return  std::make_shared<WindowHolisticSM>(machineKey , startTime, finishTime) ;
        } if(type == StateMachines::incremental) {
           return std::make_shared<WindowIncrementalSM>(machineKey , startTime, finishTime);
        } if(type == StateMachines::contAggr) {
            return std::make_shared<ConAggrSM>(machineKey , startTime, finishTime);
        } if (type == StateMachines::contJoin) {
            return std::make_shared<ConJoinSM>(machineKey , startTime, finishTime);
        }  if (type == StateMachines::intervalJoin) {
            return std::make_shared<intervalJoinSM>(machineKey , startTime, finishTime);
        }

        return {};
    }
};


#endif //GADGET_STATEMACHINEBUILDER_H
