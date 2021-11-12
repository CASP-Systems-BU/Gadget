//
// Created by Showan Asyabi on 5/23/21.
//

#ifndef GADGET_WINDOWHOLISTICSM_H
#define GADGET_WINDOWHOLISTICSM_H
#include "stateMachines.h"


class WindowHolisticSM : public StateMachines {
public:
    WindowHolisticSM(std::string currentKey, uint64_t startTime, uint64_t finishTime) {
        stateMachineKey_ = currentKey;
        startTime_ = startTime;
        finishTime_ = finishTime;
        state_ = MergeState;
        currentOpr_ = Operation::mergeOpr;
        isDone_ = false;
        currentEvent_ = nullptr;
    }

    void reset(std::shared_ptr<Event> currentEvent, bool leftStream = true, uint64_t waterMark = 1 ) override {
        currentEvent_ = currentEvent;
        state_ = MergeState;
        currentOpr_ = Operation::mergeOpr;
        isDone_ = false;
    }

    void closeStateMachine(bool leftStream = true) override {
        isDone_ = false;
        state_ = FinalGetState;
    }

    bool HasNext() override {
        if (isDone_ || (currentEvent_ == nullptr)) {
            return false;
        }

        switch (state_) {
            case StateMachines::MergeState:
                state_ = MergeState;
                currentOpr_ = Operation::mergeOpr;
                isDone_ = true;
                break;
            case FinalGetState:
                state_ = FinalDeleteState;
                currentOpr_ = Operation::getOpr;
                break;
            case FinalDeleteState:
                currentOpr_ = Operation::deleteOpr;
                isDone_ = true;
        }
        return true;
    }
};


#endif //GADGET_WINDOWHOLISTICSM_H
