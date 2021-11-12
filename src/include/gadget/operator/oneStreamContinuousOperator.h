//
// Created by Showan Asyabi on 5/23/21.
//

#ifndef GADGET_ONESTREAMCONTINUOUSOPERATOR_H
#define GADGET_ONESTREAMCONTINUOUSOPERATOR_H



#include <memory>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <map>
#include <unordered_map>

#include "include/gadget/distributions/arrival.h"
#include "include/gadget/distributions/key.h"
#include "include/gadget/distributions/value.h"
#include "include/gadget/distributions/windowLength.h"
#include "include/gadget/distributions/serviceTime.h"
#include "include/gadget/operation.h"
#include "include/gadget/statemachine/stateMachines.h"


#include "include/gadget/event/event.h"
#include "include/gadget/statemachine/stateMachineBuilder.h"
#include "include/gadget/operator/operators/operatorParameters.h"
#include "gadget/event/eventGenerator.h"


class OneStreamContinuousOperator: public Operator{

public:
#if 0
    OneStreamContinuousOperator(std::shared_ptr<OperatorParameters> params) {
        keyedOperator_ = params->keyedOperator_ ;
        windowLength_ = params->windowLength;
        latenessAllowed_ = params->latenessLength;
        latenessLength_ =  params->latenessLength;
        keyDistribution_ = params->keysDistribution;
        valueDistribution_ = params->valuesDistribution;
        eventGenerator_ = params->eventGenerator;
        serviceTimeDistribution_ = params->serviceTimeDistribution;
        stateMachineType_ = params->stateMachineType;
    }
#endif
    /**
     * This function is a main gadget driver
     * @param operationList
     * @return
     */
    bool runOperator(std::vector<std::shared_ptr<Operation>> &operationList) {

        //  get the events of the next unit of time from the event generator
        std::vector<std::shared_ptr<Event>> eventBatch;
        eventBatch.clear();
        uint64_t  time = eventGenerator_->getEventBatch(eventBatch);
        if(time == 0)  { // this indicates there no more event to process
            return false;
        }
        //  update the gadget time and get the new  watermark
        gadgetTime_ = time;
        uint64_t newWaterMark= eventGenerator_->getWaterMark();
        eventGenerator_->forgetPanes(gadgetTime_);
        // make states
        makeStates(eventBatch);
        //generate operations
        generateOperations(eventBatch, operationList);
        // update the watermark
        waterMark_ = newWaterMark;
        return true;
    }


    void  generateOperations(std::vector<std::shared_ptr<Event>> &eventBatch, std::vector<std::shared_ptr<Operation>> &operationList) {
        for(const auto &event :eventBatch) {
            std::string eventKey = (keyedOperator_)? event->key_ : "allKeys";
            for(auto &m : stateMachineDictionary_[eventKey]) {
                // reset the state machine for the current event
                m->reset(event);
                while(m->HasNext()) {
                    // make a new operation
                    auto opr =  std::make_shared<Operation>();
                    opr->oprType = m->NextOperation();
                    opr->key = m->getMachineKey();
                    opr->value = valueDistribution_->Next();
                    opr->sleepTime = serviceTimeDistribution_->Next();
                    operationList.push_back(opr);
                }
            }
        }
    }
    void makeStates(std::vector<std::shared_ptr<Event>> eventBatch) {
        for(const auto &event :eventBatch) {
            std::string eventKey = (keyedOperator_)? event->key_ : "allKeys";
            if (stateMachineDictionary_.find(eventKey) == stateMachineDictionary_.end() ) {
                auto newStateMachines = StateMachineBuilder::BuildStateMachine(stateMachineType_, keyDistribution_->Next(), 0, 0);
                if(!newStateMachines.has_value()) {
                    throw std::exception();
                }
                stateMachineDictionary_[eventKey].push_back(newStateMachines.value());
            }
        }
    }



protected:

    bool keyedOperator_;
    /***
     * the distribution keys
     */
    std::shared_ptr<Key>     keyDistribution_;
    /**
     * the distribution of values
     */
    std::shared_ptr<Value>   valueDistribution_;


    /**
     * The event generator that generates events
     */
    std::shared_ptr<EventGenerator> eventGenerator_;


    /***
     * current waterMark
     */
    uint64_t  waterMark_;

    /***
    * current gadget time
    */
    uint64_t  timeUnit_;





    /***
     * windowLength_ is the length  of the window in terms of time units
     */
    uint64_t  windowLength_;

    /***
     *  latenessLength_ is the extra amount of time that a window should wait before being closed
     */
    uint64_t  latenessLength_;

    /***
     * indicates   if the system processes  out of order events
     */
    bool latenessAllowed_;

    /***
     * the service time distribution (service rate)
     */
    std::shared_ptr<ServiceTime> serviceTimeDistribution_;

    /***
     *  We keep track of active state machines using an unordered map - key is the user key and
     *  value is the list of stata machines that are active for the user key
     *  Please note that terms stateMachine and window is used interchangeably
     */
    std::unordered_map<std::string ,std::list<std::shared_ptr<StateMachines>>> stateMachineDictionary_;


    /***
     *  we keep track of to-be-expired state machines using an unordered map - key is a finish time and the value is
     *  the list of state machine that must be closed at that finish time
     */
    std::unordered_map< uint64_t ,std::list<std::shared_ptr<StateMachines>>> expiringStateMachinesDictionary_;


    StateMachines::StateMachineType stateMachineType_;


    uint64_t gadgetTime_;
};

#endif //GADGET_ONESTREAMCONTINUOUSOPERATOR_H
