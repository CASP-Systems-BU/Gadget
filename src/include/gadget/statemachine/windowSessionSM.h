//
// Created by Showan Asyabi on 10/20/21.
//

#ifndef GADGET_WINDOWSESSIONSM_H
#define GADGET_WINDOWSESSIONSM_H


#include "stateMachines.h"


class WindowHolisticSM : public StateMachines {
public:
    WindowHolisticSM(std::string currentKey, uint64_t startTime, uint64_t finishTime) {
        stateMachineKey_ = currentKey;

        startTime_ = startTime;
        finishTime_ = finishTime;
        state_ = GetMetaState;
        windowKey_ = currentKey;
        currentOpr_ = Operation::mergeOpr;
        isDone_ = false;
        currentEvent_ = nullptr;
    }

    void reset(std::shared_ptr<Event> currentEvent, bool leftStream = true, uint64_t waterMark = 1 ) override {
        currentEvent_ = currentEvent;
        state_ = GetMetaState;
        currentOpr_ = Operation::getOpr;
        isDone_ = false;
        windowKey_ = currentEvent;
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
            case StateMachines::GetMetaState:
                state_ = MergeState;
                currentOpr_ = Operation::getOpr;
                stateMachineKey_ = "meta" + currentEvent_->key_;
                isDone_ = false;
                break;
            case StatesType::MergeState
                state_ = MergeMetaState;
                currentOpr_ = Operation::mergOpr;
                stateMachineKey_ = windowKey_;
                isDone_ = false;
                break;
            case StatesType::MergeMetaState
                //state_ = MergeMetaState;
                currentOpr_ = Operation::mergOpr;
                stateMachineKey_ =  "meta" + currentEvent_->key_;
                isDone_ = true;
                break;
            case FinalGetState:
                state_ = DeleteMetaState;
                currentOpr_ = Operation::getOpr;
                stateMachineKey_ = windowKey_;
                isDone_ = false;
                break;
            case DeleteMetaState :
                currentOpr_ = Operation::deleteOpr;
                stateMachineKey_ = "meta" + currentEvent_->key_;
                isDone_ = true;
                break;
            case StateMachines::FinalDeleteState :
                currentOpr_ = Operation::deleteOpr;
                stateMachineKey_ = windowKey_;
                isDone_ = true;
                break;
        }
        return true;
    }


public:
    std::string  windowKey_;

};

#endif //GADGET_WINDOWSESSIONSM_H
