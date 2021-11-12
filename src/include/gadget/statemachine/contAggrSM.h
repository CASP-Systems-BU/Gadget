//
// Created by Showan Asyabi on 5/23/21.
//

#ifndef GADGET_CONTAGGRSM_H
#define GADGET_CONTAGGRSM_H
#include "stateMachines.h"

class ConAggrSM : public StateMachines {
public:
    ConAggrSM(std::string currentKey, uint64_t startTime, uint64_t finishTime) {
        stateMachineKey_ = currentKey;
        startTime_ = startTime;
        finishTime_ = finishTime;
        state_ = GetState;
        currentOpr_ = Operation::getOpr;
        isDone_ = false;
        currentEvent_ = nullptr;
    }

    void reset(std::shared_ptr<Event> currentEvent, bool leftStream = true, uint64_t waterMark = 1) override {
        currentEvent_ = currentEvent;
        state_ = GetState;
        currentOpr_ = Operation::getOpr;
        isDone_ = false;
    }

    void closeStateMachine( bool leftStream = true) override {
        // not defined in cont. aggr.
    }

// here there is no final state
    bool HasNext() {
        if(isDone_ ||  (currentEvent_ == nullptr)) {
            return false;
        }

        switch(state_) {
            case GetState:
                state_ = PutState;
                currentOpr_ = Operation::getOpr;
                break;
            case PutState:
                state_ = GetState;
                currentOpr_ =  Operation::putOpr;
                // This round is done
                isDone_ = true;
                break;
        }
        return true;

    }


};

#endif //GADGET_CONTAGGRSM_H
