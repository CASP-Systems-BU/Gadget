//
// Created by Showan Asyabi on 9/20/21.
//

#ifndef GADGET_ONESTREAMSESSIONWINDOWOPERATOR_H
#define GADGET_ONESTREAMSESSIONWINDOWOPERATOR_H


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


class OneStreamSessionWindowOperator:  public Operator {
#if 0
    public:
    OneStreamWindowOperator(std::shared_ptr<OperatorParameters> params) {
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
    /// get all tne needed windows
    virtual void  assignWindows(const std::shared_ptr<Event> &event, std::vector<uint64_t> &windowStartTimes) = 0;


    /**
     * This function is a main gadget driver.
     * 1) it gets the next batch of events from the event generator
     * 2) update the watermark
     * 3) make the needed windows
     * 4) generate state store operations
     * 5) conclude expired windows
     *
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
        // update the gadget time and get the new  watermark
        gadgetTime_ = time;
        uint64_t newWaterMark= eventGenerator_->getWaterMark();
        eventGenerator_->forgetPanes(gadgetTime_);
        //  make windows for the new event batch
        makeWindows(eventBatch);

        generateOperations(eventBatch, operationList);

        for (int time = waterMark_ + 1  ; time <= newWaterMark; time++) {
            expireStateMachines(time, operationList);
        }
        waterMark_ = newWaterMark;
        return true;
    }


    /**
    *  get all needed state machines (windows) for current event batch
    * @param eventBatch
    * @param stateMachineList
    */
    void  generateOperations(std::vector<std::shared_ptr<Event>> &eventBatch, std::vector<std::shared_ptr<Operation>> &operationList) {
        for(const auto &event :eventBatch) {
            std::string eventKey = (keyedOperator_)? event->key_ : "allKeys";
            for(auto &m : stateMachineDictionary_[eventKey]) {
                if(event->eventTime_ >= m->getStartTime() && event->eventTime_ < m->getFinishTime()) {
                    m->reset(event);
                    while(m->HasNext()) {
                        auto opr = std::make_shared<Operation>();
                        opr->oprType = m->NextOperation();
                        opr->key = m->getMachineKey();
                        opr->value = valueDistribution_->Next();
                        opr->sleepTime = serviceTimeDistribution_->Next(); // fixme
                        operationList.push_back(opr);
                    }
                }
            }
        }
    }




     void makeWindows(std::vector<std::shared_ptr<Event>> eventBatch) {
        for(const auto &event :eventBatch) {
            std::vector<uint64_t> neededWindows; // this vector holds the start time of needed windows
            neededWindows.clear();
            assignWindows(event, neededWindows); // This will assign a window [event.eventTime , even.eventTime + gap]
            //std::sort (neededWindows.begin(), neededWindows.end()); no need for this line ; there is only one window
            std::string eventKey = (keyedOperator_)? event->key_ : "allKeys";
            // for each window if it does not exist, make it and push it
            for (auto const &wStartTime :neededWindows) { // there is only one window
                uint64_t windowFinishTime = wStartTime + windowLength_;
                if (stateMachineDictionaryTimeProgress_.find(eventKey) !=
                    stateMachineDictionaryTimeProgress_.end()) { // fixme
                    if (stateMachineDictionaryTimeProgress_[eventKey] >= windowFinishTime) {
                        continue;
                    }
                }
                if (stateMachineDictionary_.find(eventKey) != stateMachineDictionary_.end()) {
                    for (auto w : stateMachineDictionary_[eventKey]) {
                        if(wStartTime >= w->getStartTime() && windowFinishTime <= w->getFinishTime()) {
                            // the needed window exists. No further action is needed; return
                            return;
                        }
                    }
                }

                // if we are here, it means, the needed window does exist, we need to make it. This may trigger merges
                auto newStateMachines = StateMachineBuilder::BuildStateMachine(stateMachineType_, keyDistribution_->Next() // fixme stateMachineType_
                        , wStartTime, windowFinishTime);
                stateMachineDictionary_[eventKey].push_back(newStateMachines.value());
                auto stateMachineVector = stateMachineDictionary_[eventKey];
                stateMachineVector.sort();
                //std::sort (stateMachineVector.begin(), stateMachineVector.end());

                // check if windows can be merged
                /*
                std::sort (stateMachineDictionary_[eventKey].begin(), stateMachineDictionary_[eventKey].end(),  [ ]( const auto& lhs, const auto& rhs )
                {
                    //return lhs->getStartTime() <=  rhs->getStartTime();
                    return true;
                });
                 */

                bool merge = true, firstIteration = true;

                std::shared_ptr<StateMachines> lastWindow = NULL;
                uint64_t  prvFinishTime = 0; // the finish time of the last window
                while(merge) {
                    merge = false;
                    for (auto win : stateMachineDictionary_[eventKey]) {
                        if(firstIteration) {
                            firstIteration = false;
                            lastWindow = win;
                            prvFinishTime =  win->getFinishTime();

                        } else {
                            if (win->getStartTime() - prvFinishTime  < windowLength_) { // here we use windowLength_ for window gap in session opertaors
                                // a merge should happen
                                // adjust the finish time of the last window
                                lastWindow ->setFinishTime (win->getFinishTime());
                                // delete this window (the last one now represent both)
                                stateMachineDictionary_[eventKey].remove(win);
                                merge = true;
                                firstIteration = true;
                                break; // break from the for loop to traverse the list again

                            } else {
                                lastWindow = win;
                                prvFinishTime = win->getFinishTime();
                            }

                        }
                    }
                }


                // windows are sorted.  this  means the  needed window does not exist
                /*if (stateMachineDictionary_.find(eventKey) == stateMachineDictionary_.end() ||  stateMachineDictionary_[eventKey].back()->getFinishTime() < windowFinishTime) {
                    auto newStateMachines = StateMachineBuilder::BuildStateMachine(stateMachineType_, keyDistribution_->Next()
                            , wStartTime, windowFinishTime);
                    if(!newStateMachines.has_value()) {
                        throw std::exception();
                    }
                    stateMachineDictionary_[eventKey].push_back(newStateMachines.value());
                    stateMachineDictionaryTimeProgress_[eventKey] = windowFinishTime;
                    if (latenessAllowed_) {
                        expiringStateMachinesDictionary_[windowFinishTime + latenessLength_].push_back(newStateMachines.value());
                    } else {
                        expiringStateMachinesDictionary_[windowFinishTime].push_back(newStateMachines.value());
                    }
                } */
            }
        }
    }




    // Session window - John version
    void makeWindows2(std::vector<std::shared_ptr<Event>> eventBatch) {

        for(const auto &event :eventBatch) {
            std::string eventKey = event->key_;
                    // Make sure all mappings have been updated properly after processing the last event
            assert(mappingsToDelete.size() == 0);
            assert(assignedStateMachines.size() == 0);

            // The current input event
            //auto event = inputBuffer[eventIndex];
            // cout << "Current event time " << event->eventTime_ << " and key " << event->key_ << " at watermark " << lastWatermark << endl;
            // Ignore too late events
            //if ((event->eventTime_ + latenessLength) < prevWatermark) return;
            // Step 1: Fetch active state machines for the current event key
            //auto stateMachinesForCurrentKey = &activeStateMachines[event->key_];

            if (stateMachineDictionary_.find(eventKey) != stateMachineDictionary_.end()) {
                auto stateMachinesForCurrentKey = stateMachineDictionary_[event->key_];
                deleteMetadata = stateMachinesForCurrentKey.size() > 0;

                // The boundaries of the window the current event belongs to
                uint64_t windowStartTime = event->eventTime_,
                        windowEndTime =
                        event->eventTime_ + windowLength_; // showan we use window length for session gap
                // Step 2: Find the window the event belongs to and do the necessary window mergings (if any)
                bool merged = false;
                for (auto sm : stateMachinesForCurrentKey) {  // For each active state machine for the current key
                    if (windowStartTime <= sm->getFinishTime() &&
                        windowEndTime >= sm->getStartTime()) {
                        mergeDone = merged = true;
                        // The current session id in mappings
                        std::string sessionId = std::to_string(sm->getStartTime()) + "-" + std::to_string(sm->getFinishTime());
                        // cout << "Current session id: " << sessionId << endl;
                        // Step 2.1: Assign state machine and window id to the current event
                        auto mappings = &windowMappings[event->key_];
                        auto mapping = &mappings->at(sessionId);
                        assignedWindowForCurrentEvent = mapping->first;
                        // cout << "New assigned window for current event: " << assignedWindowForCurrentEvent << endl;
                        assignedStateMachines.push_back(sm);
                        // Step 2.2: Update mapping entries to delete ("None" means that no state merging is needed)
                        mappingsToDelete[sessionId] = std::pair<std::string, uint64_t>(std::string("None"), 0);
                        // Step 2.3: Update expiration
                        // remove the state machine from  expiringStateMachinesDictionary_
                        std::list<std::shared_ptr<StateMachines>> smList = expiringStateMachinesDictionary_[sm->getFinishTime()]; // fixme what if there is no list for this value
                        smList.remove(sm);
                        expiringStateMachinesDictionary_[sm->getFinishTime()] = smList ;
                        sm->setStartTime(std::min(windowStartTime, sm->getStartTime()));
                        sm->setFinishTime(std::max(windowEndTime, sm->getFinishTime()));
                        expiringStateMachinesDictionary_[sm->getFinishTime()].push_back(sm); // fixme
                        // Step 2.4: Update session window mappings
                        std::string newSessionId = std::to_string(sm->getStartTime()) + "-" + std::to_string(sm->getFinishTime());
                        // cout << "New session id: " << newSessionId << endl;
                        mapping->second += 1;  // Count the current event
                        mappings->insert(make_pair(newSessionId, *mapping));
                        mappings->erase(sessionId);
                    }
                    if (merged) break;
                }
                if (!merged) {


                    // Step 2: Create a new state machine for the current event and update operator state
                    /*auto new_sm = std::make_shared<SessionKeyedHolisticStateMachine>(KeyDistribution_->Next(),
                                                                                     serviceTimeDistribution,
                                                                                     windowStartTime,
                                                                                     windowEndTime,
                                                                                     shared_from_this()); */
                    auto new_sm_tmp = StateMachineBuilder::BuildStateMachine(stateMachineType_, keyDistribution_->Next() // fixme stateMachineType_
                            , windowStartTime, windowEndTime);
                    auto new_sm = new_sm_tmp.value();
                    new_sm->reset(event);
                    // Step 3.1: Assign state machine and window id to the current event
                    stateMachinesForCurrentKey.push_back(new_sm);  // Update active state machines for the current key
                    assignedStateMachines.push_back(new_sm);  // Update assigned state machines for the current event
                    assignedWindowForCurrentEvent = new_sm->getMachineKey();
                    // Step 3.2: Update expiration dictionary
                    expiringStateMachinesDictionary_[new_sm->getFinishTime()].push_back(new_sm);
                    // Step 3.3: Update session window mappings
                    std::string sessionId = std::to_string(new_sm->getStartTime()) + "-" + std::to_string(new_sm->getFinishTime());
                    auto mappings = &windowMappings[event->key_];
                    mappings->insert(make_pair(sessionId, std::pair<std::string, uint64_t>(new_sm->getMachineKey(), 1)));
                    return;
                }
                do {  // Step 4: Check if there are more windows to merge and update state accordingly
                    merged = false;
                    // cout << "Number of state machines for current key: " << stateMachinesForCurrentKey->size() << endl;
                    for (auto i = stateMachinesForCurrentKey.begin(); i != stateMachinesForCurrentKey.end(); ++i) {
                        auto stateMachine1 = *i;
                        for (auto j = next(i, 1); j != stateMachinesForCurrentKey.end(); ++j) {
                            auto stateMachine2 = *j;
                            // Check if the two windows should be merged
                            if (stateMachine2->getStartTime() <= stateMachine1->getFinishTime() &&
                                stateMachine2->getFinishTime() >= stateMachine1->getStartTime()) {
                                mergeDone = merged = true;
                                // Let's keep the state machine with the earliest start time and discard the other one
                                auto stateMachineToKeep = (stateMachine1->getStartTime() < stateMachine2->getStartTime()
                                                           ? stateMachine1 : stateMachine2);
                                auto stateMachineToDelete = (stateMachineToKeep->getMachineKey() == stateMachine1->getMachineKey()
                                                             ? stateMachine2 : stateMachine1);
                                auto sessionIdToDelete1 = std::to_string(stateMachineToKeep->getStartTime()) + "-" +
                                                          std::to_string(stateMachineToKeep->getFinishTime());
                                auto sessionIdToDelete2 = std::to_string(stateMachineToDelete->getStartTime()) + "-" +
                                                          std::to_string(stateMachineToDelete->getFinishTime());
                                auto winToKeep = stateMachineToKeep->getMachineKey();
                                // cout << "Win id to keep: " << winToKeep << endl;
                                auto winToDelete = stateMachineToDelete->getMachineKey();
                                // cout << "Win id to delete: " << winToDelete << endl;
                                auto newEnd = std::max(stateMachine1->getFinishTime(), stateMachine2->getFinishTime());
                                auto mappings = &windowMappings[event->key_];
                                auto mKeep = mappings->at(sessionIdToDelete1);
                                auto mDelete = mappings->at(sessionIdToDelete2);
                                // Step 4.1: Re-assign state machine and window id to the current event
                                if (winToDelete.compare(assignedWindowForCurrentEvent) == 0) {
                                    assignedWindowForCurrentEvent = winToKeep;
                                    assignedStateMachines.remove(stateMachineToDelete);
                                    assignedStateMachines.push_back(stateMachineToKeep);
                                }
                                // Step 4.2: Remove old state machine from the list of state machines for the current key
                                stateMachinesForCurrentKey.remove(stateMachineToDelete);
                                // Step 4.3: Update mapping entries to delete
                                mappingsToDelete[sessionIdToDelete2] = std::pair<std::string, uint64_t>(winToDelete,
                                                                                              mDelete.second);
                                // Step 4.4: Update expiration dictionary
                                //expiredStateMachines.remove(stateMachineToDelete);
                                std::list<std::shared_ptr<StateMachines>> smList = expiringStateMachinesDictionary_[stateMachineToDelete->getFinishTime()]; // fixme what if there is no list for this value
                                smList.remove(stateMachineToDelete);
                                expiringStateMachinesDictionary_[stateMachineToDelete->getFinishTime()] = smList ;
                                //expiredStateMachines->remove(stateMachineToKeep);
                                smList = expiringStateMachinesDictionary_[stateMachineToKeep->getFinishTime()]; // fixme what if there is no list for this value
                                smList.remove(stateMachineToKeep);
                                expiringStateMachinesDictionary_[stateMachineToKeep->getFinishTime()] = smList ;

                                stateMachineToKeep->setFinishTime(newEnd);
                                //expiredStateMachines->insert(stateMachineToKeep);
                                expiringStateMachinesDictionary_[stateMachineToKeep->getFinishTime()].push_back(stateMachineToKeep);

                                // Step 4.5: Update session window mappings
                                std::string newSessionId = std::to_string(stateMachineToKeep->getStartTime()) + "-" +
                                                      std::to_string(stateMachineToKeep->getFinishTime());
                                mKeep.second += mDelete.second;  // Count the merged events
                                mappings->insert(make_pair(newSessionId, mKeep));
                                mappings->erase(sessionIdToDelete1);
                                mappings->erase(sessionIdToDelete2);
                                break;
                            }
                        }
                        if (merged) break;
                    }
                } while (merged);
            } //
            assert(assignedStateMachines.size() == 1);  // There must be only one assigned state machine
            assignedStateMachines.front()->reset(event);
        }
        // cout << "Current assigned window: " << assignedWindowForCurrentEvent << endl;
    }
/***
 * this function searches the current state machines (windows) that can be closed at this time
 * The function returns all state machines tat can be closed at this time
 * @return
 */
    void  expireStateMachines(uint64_t time, std::vector<std::shared_ptr<Operation>> &operationList) {
        if (expiringStateMachinesDictionary_.find(time) !=  expiringStateMachinesDictionary_.end()) {

            for(const auto &m : expiringStateMachinesDictionary_[time]) {
                m->closeStateMachine();
                while(m->HasNext()) {
                    // make a new operation
                    auto opr =  std::make_shared<Operation>();
                    opr->oprType = m->NextOperation();
                    opr->key = m->getMachineKey();
                    opr->value = valueDistribution_->Next();
                    opr->sleepTime = serviceTimeDistribution_->Next(); // fixme
                    operationList.push_back(opr);
                }
            }

            // remove these machines from the  window dictionary
            for(const auto &m : expiringStateMachinesDictionary_[time]) {
                std::string Key = (keyedOperator_)? m->getUserKey() : "allKeys";
                auto &windows = stateMachineDictionary_[Key];
                auto itr = windows.begin();
                while (itr != windows.end()) {
                    if (m->getStartTime() == (*itr)->getStartTime() && m->getFinishTime() == (*itr)->getFinishTime()) {
                        itr = windows.erase(itr);
                    } else {
                        ++itr;
                    }
                }
                if(windows.size() == 0) {
                    stateMachineDictionary_.erase(Key) ;
                }
            }
            expiringStateMachinesDictionary_.erase(time);
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
     * slidingLength_ is the length  of sliding in terms of time units
     */
    uint64_t  slidingLength_;

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


    /***
     * this dictionary tracks the time progress for each key. I use this dictionary to prevent out of order events
     * to make a new windows if  that window has been already evaluated
     */
    std::unordered_map<std::string, uint64_t> stateMachineDictionaryTimeProgress_;


    StateMachines::StateMachineType stateMachineType_;


    uint64_t gadgetTime_;


    // John's variables

    /** A mapping from key=(start, end) to pairs of the form (winId, #elements)
    * The hashmap stores all session windows that have been merged with others due to the current event
            * These entries must be deleted from session window mappings
    */
    std::unordered_map<std::string, std::pair<std::string, uint64_t>>  mappingsToDelete;
    /**
     *
     * A mapping from outer_key='event key' to inner_key=(start, end) and from inner_key=(start, end)
     * to pairs of the form (winId, #elements)
     * This is a double hashmap that contains all open session window mappings for all event keys
     * we have seen so far
     */
    std::unordered_map<std::string, std::unordered_map<std::string, std::pair<std::string, uint64_t>>>  windowMappings;
    /***
     *
     * mergeDone is true if there was at least one merging in window mappings due to the last event
     */
    bool mergeDone;
    /***
     *
     * deleteMetadata is true if there was at least one window mapping for the current event key
     */
    bool deleteMetadata;
    /***
     *
     *  The id of the assigned window for the current event
     */
    std::string  assignedWindowForCurrentEvent;

    /***
     *
     * The list of state machines assigned to the current event (only one for session window)
     */
    std::list<std::shared_ptr<StateMachines>> assignedStateMachines;

};





#endif //GADGET_ONESTREAMSESSIONWINDOWOPERATOR_H
