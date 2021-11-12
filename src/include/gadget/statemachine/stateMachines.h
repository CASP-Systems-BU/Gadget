//
// Created by Showan Asyabi on 5/21/21.
//

#ifndef GADGET_STATEMACHINES_H
#define GADGET_STATEMACHINES_H
#include "include/gadget/event/event.h"

class  StateMachines {
public:
    enum StatesType {
        GetState = 0, PutState, MergeState, FinalGetState, FinalDeleteState, ScanState, NextState
    };

    enum StateMachineType {
        incremental = 0, holistic, intervalJoin, contAggr, contJoin
    };
    virtual ~StateMachines() = default;
    virtual bool HasNext() = 0;
    virtual  void closeStateMachine(bool leftStream = true) = 0;
    virtual   void reset(std::shared_ptr<Event> currentEvent, bool leftStream = true, uint64_t waterMark = 1) = 0;



    uint64_t getStartTime() {
        return  startTime_;
    }

    uint64_t getFinishTime() {
        return  finishTime_;
    }

    std::string getUserKey() {
        return currentEvent_->key_;
    }


    Operation::OperationType NextOperation() {
        return currentOpr_;
    }


    std::string getMachineKey() {
        return stateMachineKey_;
    }



protected:

    /**
     * startTime indicates when the state machine was born in gadget time
     */
    uint64_t  startTime_;

    /**
    * finishTime_ indicates a upper bound of  event times of events that can be processed in this window (i.e., state machine)
    */
    uint64_t  finishTime_;


    /**
     * key indicates is the the key of the state machine uses for accessing state store. All state
     * accesses stem from this state machine use this key
     */
    std::string stateMachineKey_;


    /**
     * current state of the state machine
     */
    StatesType state_;

    /**
     *  IsDone_ is true if the current round is done
     */
    bool isDone_;

    /**
     *  currentOpr_ is the current generated operation by the state machine
     */
    Operation::OperationType currentOpr_;

    /**
     * current event that is processed by the state machine
     */
    std::shared_ptr<Event> currentEvent_;



};

#endif //GADGET_STATEMACHINES_H
