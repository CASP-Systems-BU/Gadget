//
// Created by Showan Asyabi on 5/23/21.
//

#ifndef GADGET_CONJOINSM_H
#define GADGET_CONJOINSM_H
#include "stateMachines.h"
// This machine is for continuous join on borg traces

class ConJoinSM : public StateMachines {

public:
    enum ConJoinStateType {
        DeleteRightState = 0, DeleteLeftState , AddNewJob, AddNewJob1, AddNewJob2 , JOBGetState,  TaskAddState, TaskAddState2
    };
    ConJoinSM(std::string currentKey, uint64_t startTime, uint64_t finishTime) {
        stateMachineKey_ = currentKey;
        startTime_ = startTime;
        finishTime_ = finishTime;
        isDone_ = false;
        currentEvent_ = nullptr;
        taskListSize_ = 0;
        jobListSize_ = 0;
    }
    //  the driver calls  reset(event, false) if the events are form right stream
    void reset(std::shared_ptr<Event> currentEvent, bool leftStream = true, uint64_t waterMark = 1) override {
        // left stream are job events
        if(leftStream) {
            if(currentEvent->type_ == "FINISH") {
                State_ =  DeleteLeftState;
            } else {
                    State_ =  AddNewJob;
            }
        } else {  // event come from the right stream - task stream
                State_ =  TaskAddState;

        }
        currentEvent_ = currentEvent;
        isDone_ = false;
    }

    void closeStateMachine(bool leftStream = true) override {
        // not defined for cont join
    }

    bool HasNext() override {
        if (isDone_ || (currentEvent_ == nullptr)) {
            return false;
        }
        switch (State_) {
            case DeleteLeftState:
                jobListSize_ = 0;
                State_ = DeleteRightState;
                currentOpr_ = Operation::deleteOpr;
                isDone_ = false ;
                break;
            case DeleteRightState:
                taskListSize_ = 0;
                State_ = DeleteRightState;
                currentOpr_ = Operation::deleteOpr;
                isDone_ = true ;
                break;
            case AddNewJob:
                State_ = AddNewJob1;
                currentOpr_ = Operation::getOpr;
                isDone_ = false;
                break;
            case AddNewJob1:
                State_ = AddNewJob2;
                currentOpr_ = Operation::deleteOpr;
                isDone_ = false;
                break;
            case AddNewJob2:
                State_ = AddNewJob2;
                jobListSize_ ++;
                currentOpr_ = Operation::putOpr;
                isDone_ = true;
                break;
            case TaskAddState:
                State_ = TaskAddState2;
                currentOpr_ = Operation::getOpr;
                if(jobListSize_ > 0) {
                    isDone_ = true;
                    State_ = TaskAddState;
                } else {
                    isDone_ = false;
                    State_ = TaskAddState2;
                }
                break;
            case TaskAddState2:
                State_ = TaskAddState2;
                currentOpr_ = Operation::mergeOpr;
                taskListSize_ ++;
                isDone_ = true;
                break;
        }
        return true;
    }

private:
    uint64_t taskListSize_;
    uint64_t jobListSize_;
    ConJoinStateType State_;

};

#endif //GADGET_CONJOINSM_H
