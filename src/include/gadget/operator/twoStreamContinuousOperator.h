//
// Created by Showan Asyabi on 5/23/21.
//

#ifndef GADGET_TWOSTREAMCONTINUOUSOPERATOR_H
#define GADGET_TWOSTREAMCONTINUOUSOPERATOR_H


#include <memory>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <map>
#include <unordered_map>
#include <climits>

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



    class TwoStreamContinuousOperator: public Operator {

    public:
        /**
         * This function is a main gadget driver that  does follows when it is called:
         * 1)
         * @param operationList
         * @return
         */
        bool runOperator(std::vector<std::shared_ptr<Operation>> &operationList) {

            //  get the events of the next unit of time from the event generator
            std::vector<std::shared_ptr<Event>> leftStreamEventBatch;
            std::vector<std::shared_ptr<Event>> rightStreamEventBatch;
            leftStreamEventBatch.clear();
            rightStreamEventBatch.clear();
            uint64_t  leftStreamTime = eventGenerator_->getEventBatch(leftStreamEventBatch);
            uint64_t  rightStreamTime = eventGenerator2_->getEventBatch(rightStreamEventBatch);
            if(leftStreamTime == 0 &&  rightStreamTime == 0)  { // this indicates there are no more events to process
                return false;
            }

            //  update the gadget time and get the new  watermark
            uint64_t leftStreamWaterMark= (leftStreamTime)? eventGenerator_->getWaterMark(): LLONG_MAX;
            uint64_t rightStreamWaterMark= (rightStreamTime)? eventGenerator_->getWaterMark(): LLONG_MAX;
            // update the watermark
            waterMark_  = std::min(leftStreamWaterMark, rightStreamWaterMark);

            eventGenerator_->forgetPanes(leftStreamTime);
            eventGenerator2_->forgetPanes(rightStreamTime);

            if(leftStreamTime) {
                makeStates(leftStreamEventBatch);
            }
            if(rightStreamTime) {
                makeStates(rightStreamEventBatch);
            }

            if(leftStreamTime) {
                generateOperations(leftStreamEventBatch, operationList, true);
            }
            if(rightStreamTime) {
                generateOperations(rightStreamEventBatch, operationList, false);
            }

            return true;
        }


        void  generateOperations(std::vector<std::shared_ptr<Event>> &eventBatch, std::vector<std::shared_ptr<Operation>> &operationList , bool isLeftStream) {
            for(const auto &event :eventBatch) {
                std::string eventKey = (keyedOperator_)? event->key_ : "allKeys";
                for(auto &m : stateMachineDictionary_[eventKey]) {
                    // check if the state machine  is suitable for  for the event // fixme
                    // reset the state machine for the current event
                    m->reset(event, isLeftStream, waterMark_);
                    while(m->HasNext()) {
                        auto opr =  std::make_shared<Operation>();
                        opr->oprType = m->NextOperation();
                        opr->key = m->getMachineKey();
                        opr->value = valueDistribution_->Next();
                        //opr->sleepTime = serviceTimeDistribution_->Next(); // fixme
                        operationList.push_back(opr);
                    }
                }
            }
        }

        void makeStates(std::vector<std::shared_ptr<Event>> eventBatch) {
            for(const auto &event :eventBatch) {
                std::string eventKey = (keyedOperator_)? event->key_ : "allKeys";
                if (stateMachineDictionary_.find(eventKey) == stateMachineDictionary_.end() ) {
                    auto newStateMachines = StateMachineBuilder::BuildStateMachine(stateMachineType_, keyDistribution_->Next(), lowerBound_, upperBound_);
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

        /**
        * The second event generator for two streams operators(e.g., join)
        */
        std::shared_ptr<EventGenerator> eventGenerator2_;

        /***
         * current waterMark
         */
        uint64_t  waterMark_;

        /***
        * current gadget time
        */
        uint64_t  timeUnit_;

        /***
    * this is used for interval join
    */
        uint64_t  upperBound_ = 0;


        /***
        * This is used for interval join
        */
        uint64_t  lowerBound_ = 0;


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


#endif //GADGET_TWOSTREAMCONTINUOUSOPERATOR_H
