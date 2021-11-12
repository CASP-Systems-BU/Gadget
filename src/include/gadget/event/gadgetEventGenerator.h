//
// Created by Showan Asyabi on 5/21/21.
//

#ifndef GADGET_GADGETEVENTGENERATOR_H
#define GADGET_GADGETEVENTGENERATOR_H

#include <memory>
#include <algorithm>


#include <unordered_map>
#include <random>
#include <cassert>

#include "include/gadget/event/event.h"
#include "include/gadget/distributions/arrival.h"
#include "include/gadget/event/eventGenerator.h"
#include "include/gadget/event/eventGeneratorParameters.h"

class GadgetEventGenerator : public EventGenerator {
public:
    GadgetEventGenerator(std::shared_ptr<EventGeneratorParameters> params) {
        keyPopularity_ = params->keyPopularity;
        eventOccurrenceTimeDistribution_ = params->eventOccurrenceTimeDistribution;
        arrivalTimeDistribution_ = params->arrivalTimesDistribution;

        // out of order  events
        outOfOrderPercentage_ = params->outOfOrderPercentage;
        latenessThreshold_ = params->latenessThreshold;

        // the occurrence time of the first event is set to zero
        eventOccurrenceTime_ = 0;
        lastForgottenTimeUnit_ = 0;
        currentTimeUnitEvents_.clear();
        activeTimeUnitVector_.clear();
        // Gedget clock is set to zero
        gadgetTimeUnit_ = 0;
        totalNumOfGeneratedEvents_ = 0;

        waterMarkFrequency_ = params->waterMarkFrequency;

    }
    /***
     *
     * This function creates  a batches of events  belonging to the next unit of time and returns its time unit - Some the events might be out of order
     */
    uint64_t generateNextPane_outOfOrder() override {

        // increase the time unit. gadgetTimeUnit_ is the current maximum time
        gadgetTimeUnit_ ++;


        currentTimeUnitEvents_.clear();
        // add out of order  events that are supposed appear in this unit of time
        if(outOfOrderMap.find(gadgetTimeUnit_) != outOfOrderMap.end()) {
            for (auto e : outOfOrderMap[gadgetTimeUnit_]) {
                currentTimeUnitEvents_.push_back(e);
            }
            outOfOrderMap.erase(gadgetTimeUnit_);
        }

        while(true) {
            // make a new Event
            std::shared_ptr<Event> newEvent = std::make_shared<Event>(); // fixme -   need a constructor for  the event class
            newEvent->key_ = keyPopularity_->Next();

            eventOccurrenceTime_ = eventOccurrenceTime_ + eventOccurrenceTimeDistribution_->Next();
            /**
            * Here a arrivalTime_ of  a new event is relative. It indicates for how
            * long the streaming system should wait until this event arrives
            */
            newEvent->arrivalTime_ = arrivalTimeDistribution_->Next();
            newEvent->eventTime_ = eventOccurrenceTime_;

            // check if this event is chosen to be out order
            std::random_device rd;
            std::mt19937 gen (rd());
            std::uniform_int_distribution<> ifChooseDistrib(0 , 100);
            // fixme  LateEventDestinationDistrib to be chosen by user - here we use a uniform distribution
            std::uniform_int_distribution<> destinationDistrib(gadgetTimeUnit_ + 1,gadgetTimeUnit_ + latenessThreshold_);
            bool chosenToBeLate = round(ifChooseDistrib(gen)) < outOfOrderPercentage_;

            uint64_t  destinationTimeUnit;
            // the event is chosen to be  out of order
            if(chosenToBeLate || eventOccurrenceTime_ >= gadgetTimeUnit_) {
                // determine the  pane that this event should go
                if (chosenToBeLate) {
                    destinationTimeUnit = round(destinationDistrib(gen));
                } else {
                    destinationTimeUnit =  eventOccurrenceTime_/ gadgetTimeUnit_;
                }

                if(outOfOrderMap.find(destinationTimeUnit) != outOfOrderMap.end()) {
                    outOfOrderMap[destinationTimeUnit].push_back(newEvent);
                } else {
                    outOfOrderMap[destinationTimeUnit] = {newEvent};
                }
            }
            // This pane is finished
            if (eventOccurrenceTime_ >= gadgetTimeUnit_) {
                activeTimeUnitVector_.push_back(currentTimeUnitEvents_);
                break;
            }
            // if this event is not a late (out of order )  event add it to the current pane
            if(!chosenToBeLate) {
                currentTimeUnitEvents_.push_back(newEvent);
            }
        }
        return gadgetTimeUnit_;
    }

    /**
     * generateNextPane() creates  a batches of events  belonging to the next unit of time and returns its time unit
     */
    uint64_t generateNextPane() override{

        //  enhance the  the time
        gadgetTimeUnit_ ++;  /// the  maximum time


        while(true) {
            // make a new Event
            std::shared_ptr<Event> newEvent = std::make_shared<Event>(); // fixme - we have to constructor for  the event class
            newEvent->key_ = keyPopularity_->Next();
            eventOccurrenceTime_ = eventOccurrenceTime_ + eventOccurrenceTimeDistribution_->Next();
            /**
             * Here a arrivalTime_ of  a new event is relative. It indicates for how
             * long the system should wait until this event arrives
             */
            newEvent->arrivalTime_ = arrivalTimeDistribution_->Next();
            newEvent->eventTime_ = eventOccurrenceTime_;
            if (eventOccurrenceTime_ >= gadgetTimeUnit_) {
                // fixme maybe its better if we have an approach like pervious  function- it is possible that a  time unit does not have any event
                activeTimeUnitVector_.push_back(currentTimeUnitEvents_);
                currentTimeUnitEvents_.clear();
                currentTimeUnitEvents_.push_back(newEvent);
                break;
            }
            currentTimeUnitEvents_.push_back(newEvent);
        }
        return gadgetTimeUnit_;
    }


    void printPane(uint64_t timeUint) override {
        assert(timeUint > lastForgottenTimeUnit_);
        uint64_t index =  timeUint - lastForgottenTimeUnit_ - 1;
        for (auto e : activeTimeUnitVector_[index]) {
            std::cout << e->eventTime_ << "---";
        }
        std::cout<<std::endl;
    }


    /***
     * This function returns the events  of the timeUint
     */
    std::vector<std::shared_ptr<Event>> getPane(uint64_t timeUint) override {
        assert(timeUint > lastForgottenTimeUnit_);
        uint64_t index =  timeUint - lastForgottenTimeUnit_ - 1;
        totalNumOfGeneratedEvents_ +=  activeTimeUnitVector_[index].size();
        std::cout<< "pane size:" << activeTimeUnitVector_[index].size() << std::endl;
        return  activeTimeUnitVector_[index];
    }
    /***
     *
     * @param timeUint
     * This function erase the time units from start to the timeUint
     * return false if  up to the timeUint is already erased
     */
    bool forgetPanes(uint64_t timeUint) override {
        /*if (timeUint <= lastForgottenTimeUnit_ ) {
            return false;
        }
        uint64_t index =  timeUint - lastForgottenTimeUnit_;
        activeTimeUnitVector_.erase( activeTimeUnitVector_.begin(), activeTimeUnitVector_.begin() + index); // last element is not deleted
        lastForgottenTimeUnit_ = timeUint;*/
        return true;
    }

    uint64_t  getWaterMark() {
        // update watermark
        if (gadgetTimeUnit_ % waterMarkFrequency_ == 0) {
            currentWaterMark_ = gadgetTimeUnit_;
        }
        return  currentWaterMark_;
    }
    uint64_t getEventBatch(std::vector<std::shared_ptr<Event>> &eventBatch) {

        gadgetTimeUnit_ ++;  /// // the current maximum time
        eventBatch .clear();

        if(outOfOrderPercentage_ > 0 ) {
            // add out of order  events that are supposed appear in this unit of time
            if(outOfOrderMap.find(gadgetTimeUnit_) != outOfOrderMap.end()) {
                for (auto e : outOfOrderMap[gadgetTimeUnit_]) {
                    eventBatch.push_back(e);
                }
                outOfOrderMap.erase(gadgetTimeUnit_);
            }

            while(true) {
                // make a new Event
                std::shared_ptr<Event> newEvent = std::make_shared<Event>(); // fixme -   need a constructor for  the event class
                newEvent->key_ = keyPopularity_->Next();

                eventOccurrenceTime_ = eventOccurrenceTime_ + eventOccurrenceTimeDistribution_->Next();
                /**
                * Here a arrivalTime_ of  a new event is relative. It indicates for how
                * long the streaming system should wait until this event arrives
                */
                newEvent->arrivalTime_ = arrivalTimeDistribution_->Next();
                newEvent->eventTime_ = eventOccurrenceTime_;

                // check if this event is chosen to be out order
                std::random_device rd;
                std::mt19937 gen (rd());
                std::uniform_int_distribution<> ifChooseDistrib(0 , 100);
                // fixme  LateEventDestinationDistrib to be chosen by user - here we use a uniform distribution
                std::uniform_int_distribution<> destinationDistrib(gadgetTimeUnit_ + 1,gadgetTimeUnit_ + latenessThreshold_);
                bool chosenToBeLate = round(ifChooseDistrib(gen)) < outOfOrderPercentage_;

                uint64_t  destinationTimeUnit;
                // the event is chosen to be  out of order
                if(chosenToBeLate || eventOccurrenceTime_ >= gadgetTimeUnit_) {
                    // determine the  pane that this event should go
                    if (chosenToBeLate) {
                        destinationTimeUnit = round(destinationDistrib(gen));
                    } else {
                        destinationTimeUnit =  eventOccurrenceTime_/ gadgetTimeUnit_;
                    }

                    if(outOfOrderMap.find(destinationTimeUnit) != outOfOrderMap.end()) {
                        outOfOrderMap[destinationTimeUnit].push_back(newEvent);
                    } else {
                        outOfOrderMap[destinationTimeUnit] = {newEvent};
                    }
                }
                // This pane is finished
                if (eventOccurrenceTime_ >= gadgetTimeUnit_) {
                    //activeTimeUnitVector_.push_back(currentTimeUnitEvents_);
                    break;
                }
                // if this event is not a late (out of order )  event add it to the current pane
                if(!chosenToBeLate) {
                    eventBatch.push_back(newEvent);
                }
            }
            return gadgetTimeUnit_;

        } else {

            while (true) {
                //std::cout << "here" << std::endl;
                // make a new Event
                std::shared_ptr<Event> newEvent = std::make_shared<Event>(); // fixme - we have to constructor for  the event class
                newEvent->key_ = keyPopularity_->Next();
                eventOccurrenceTime_ = eventOccurrenceTime_ + eventOccurrenceTimeDistribution_->Next();
                /**
                 * Here a arrivalTime_ of  a new event is relative. It indicates for how
                 * long the system should wait until this event arrives
                 */
                newEvent->arrivalTime_ = arrivalTimeDistribution_->Next();
                newEvent->eventTime_ = eventOccurrenceTime_;
                if (eventOccurrenceTime_ >= gadgetTimeUnit_) {
                    // fixme maybe its better if we have an approach like pervious  function- it is possible that a  time unit does not have any event
                    //activeTimeUnitVector_.push_back(currentTimeUnitEvents_);
                    //currentTimeUnitEvents_.clear();
                    //currentTimeUnitEvents_.push_back(newEvent);
                    break;
                }
                eventBatch.push_back(newEvent);
            }
        }
        //std::cout << "return" << std::endl;
        return gadgetTimeUnit_;
    }


public:
    // the time unit that is forgotten (i.e, erased for the time unit vector because it is no longer needed)
    uint64_t lastForgottenTimeUnit_;
    // a vector that hold all events belonging the current gadget time unit
    std::vector<std::shared_ptr<Event>> currentTimeUnitEvents_;
    // the time the an event occurred  in the source
    double eventOccurrenceTime_;
    // the current time of the  gadget clock
    uint64_t  gadgetTimeUnit_;
    // a vector that holds all active time unit's events
    std::vector< std::vector<std::shared_ptr<Event>>> activeTimeUnitVector_;
    // the distribution of events' occurrence time
    std::shared_ptr<Arrival> eventOccurrenceTimeDistribution_;
    // the distribution of events' arrival time
    std::shared_ptr<Arrival> arrivalTimeDistribution_;
    // the distribution of events' keys
    std::shared_ptr<KeyPopularity> keyPopularity_;
    // out of order events support
    // the  time threshold for accepting out of order events
    uint64_t latenessThreshold_;
    // the  the fraction of events that are going to be late
    uint8_t outOfOrderPercentage_;
    // a map that hold events belonging to the future time units
    std::unordered_map<uint64_t, std::vector<std::shared_ptr<Event>>> outOfOrderMap;
    uint64_t  totalNumOfGeneratedEvents_;

    uint64_t  waterMarkFrequency_;
    uint64_t  currentWaterMark_;
};


#endif //GADGET_GADGETEVENTGENERATOR_H
