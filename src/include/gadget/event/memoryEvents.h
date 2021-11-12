//
// Created by Showan Asyabi on 5/27/21.
//

/**
 * This class is for hard coded memory events. This class is mainly used for tests
 */

#ifndef GADGET_MEMORYEVENTS_H
#define GADGET_MEMORYEVENTS_H


#include <memory>
#include <chrono>

#include "event.h"
#include "include/gadget/event/eventGeneratorParameters.h"

class  MemoryEvents: public EventGenerator {
public:
    MemoryEvents(std::shared_ptr<EventGeneratorParameters> params) {
        generatedEvents.clear();
        waterMarkFrequency_ = params->waterMarkFrequency;
        generatedEvents = params->memEvents;
        gadgetTimeUnit_ = generatedEvents[0]->eventTime_;
        generatedEventsIndex_ = 0;




    }
    uint64_t generateNextPane_outOfOrder() override {
        // no need for this function when events come form a  trace file
        return 0;
    }


    // fixme remove the following functions
    uint64_t generateNextPane () override {
        return  1;
    }

    std::vector<std::shared_ptr<Event>> getPane(uint64_t watermark)  override {
        std::vector<std::shared_ptr<Event>> v;
        return v;
    }
    bool  forgetPanes (uint64_t time)  override {
        return true;
    }
    void printPane(uint64_t watermark)  override {

    }


    uint64_t  getWaterMark() override{
        // update watermark
        if (gadgetTimeUnit_ % waterMarkFrequency_ == 0) {
            currentWaterMark_ = gadgetTimeUnit_;
        }
        return  currentWaterMark_;
    }

    uint64_t getEventBatch(std::vector<std::shared_ptr<Event>> &eventBatch)  override {
        gadgetTimeUnit_ ++;  /// the current maximum time
        eventBatch .clear();
        while(generatedEventsIndex_ < generatedEvents.size()) {
            auto newEvent = generatedEvents[generatedEventsIndex_];
            uint64_t  eventOccurrenceTime_ =  newEvent->eventTime_;

            if (eventOccurrenceTime_ >= gadgetTimeUnit_) {
                break;
            }
            eventBatch.push_back(newEvent);
            generatedEventsIndex_++;
        }

        if (generatedEventsIndex_ >=  generatedEvents.size()) {
            if(eventBatch.size() > 0) {
                return gadgetTimeUnit_;
            } else {
                return 0; // indicates no event is left
            }
        }

        return gadgetTimeUnit_;
    }


private:
    std::vector<std::shared_ptr<Event>> generatedEvents;
    uint64_t  gadgetTimeUnit_; // gadget clock
    uint64_t  generatedEventsIndex_;
    uint64_t  waterMarkFrequency_;
    uint64_t  currentWaterMark_;

};




#endif //GADGET_MEMORYEVENTS_H
