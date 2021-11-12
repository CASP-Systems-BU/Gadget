//
// Created by Showan Asyabi on 5/21/21.
//

#ifndef GADGET_TRACEEVENTS_H
#define GADGET_TRACEEVENTS_H

#include <memory>
#include <chrono>

#include "event.h"
#include "include/gadget/event/eventGeneratorParameters.h"

class  TraceEvents: public EventGenerator {
public:
    TraceEvents(std::shared_ptr<EventGeneratorParameters> params) {
        generatedEvents.clear();

        //
        uint32_t  keyIndex = 0 ;
        uint32_t  evenTimeIndex = 0;
        uint32_t  typeIndex = 0;
        waterMarkFrequency_ = params->waterMarkFrequency; // fixme
/*
        switch (params->filetype) {
            case EventGeneratorParameters::MyFile:
                keyIndex = 2;
                evenTimeIndex = 0;
                break;
            case EventGeneratorParameters::BorgJobFile:
                keyIndex = 0;
                evenTimeIndex = 1;
                break;
            case EventGeneratorParameters::BorgTaskFile:
                keyIndex = 0;
                evenTimeIndex = 2;
                break;
        }
        */
        keyIndex = params->keyIndex;
        evenTimeIndex = params->eventTimeIndex;
        typeIndex = params->typeIndex;



        // read the event file
        std::ifstream fileStream(params->eventFilePath);
        std::string line;
        std::vector<std::string> row;
        long double  eventTime;
        double  arrivalTime;
        std::string eventType;
        std::string eventKey;
        while (std::getline(fileStream, line)) {
            std::istringstream is_line(line);
            std::string field;
            while (std::getline(is_line, field,',')) {
                row.push_back(field);
            }
            row.clear();
            try {
                /*if (params->filetype == EventGeneratorParameters::MyFile) {
                    eventTime = std::stod(row[evenTimeIndex]);
                    arrivalTime = std::stod(row[1]);  // fixme
                    eventKey = row[keyIndex];
                    eventType = std::stoll(row[3]); // fixme
                } else { */
                uint64_t timeInMicro = std::stol(row[evenTimeIndex]);
                //we assuming  me that time in the event file is in microsecond, here we get event time in seconds. In other words, we define the gadget unit of time to be second
                eventTime = timeInMicro / 1000000.0;
                //eventTime = timeInMicro / 1000.0; // millisecond  time (join interval)
                arrivalTime = 0; // we read events from the file ; therefore, there is no event time
                eventKey = row[keyIndex];
                eventType = row[typeIndex];
                //   }
            } catch (...) {
                std::cout << "Error: the event file cannot be parsed: Please check if the the index number of different fields are correct " << std::endl;
                continue;
            }

            auto newEvent = std::make_shared<Event>();
            newEvent->key_ = eventKey;
            newEvent->arrivalTime_ = arrivalTime;
            newEvent->eventTime_ = eventTime;
            newEvent->type_ = eventType;
            generatedEvents.push_back(newEvent);
        }
        fileStream.close();

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

#endif //GADGET_TRACEEVENTS_H
