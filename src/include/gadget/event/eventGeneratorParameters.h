//
// Created by Showan Asyabi on 4/13/21.
//

#ifndef GADGET_EVENTGENERATORPARAMETERS_H
#define GADGET_EVENTGENERATORPARAMETERS_H


#include "include/gadget/distributions/arrival.h"
#include "include/gadget/distributions/keyPopularity.h"
#include "include/gadget/distributions/windowLength.h"


class  EventGeneratorParameters {
public:
      EventGeneratorParameters() {
    }
    enum FileTypes {
        MyFile = 0, BorgTaskFile, BorgJobFile
    };

public:
    // The distribution of occurrence time of events  in the source
    std::shared_ptr<Arrival> eventOccurrenceTimeDistribution;
    // The distribution of arrival time of events - (when they arrive at the streaming systems)
    std::shared_ptr<Arrival> arrivalTimesDistribution;
    // The  distribution of the of the keys of the events
    std::shared_ptr<KeyPopularity> keyPopularity;

    //  event generator can create out of order events
    uint64_t latenessThreshold; // unit of time
    double outOfOrderPercentage;

    // if events are located in the file. eventFilePath is the path of that file
    std::string eventFilePath;
    // if events are located in the file,  the index for the eventTime ( Index = column number)
    uint16_t  eventTimeIndex;
    // if events are located in the file,   what is the index for the  event key ( Index = column number)
    uint16_t keyIndex;
    // if events are located in the file,  what is  the index for the  event type ( Index = column number)
    uint16_t  typeIndex;
    // this filed indicates  the  frequency of watermark. For example, if it is set to 5, every 5 unit of time, the watermark is updated
    uint16_t waterMarkFrequency;
    // used for tests- We make some events and pass it to the memory events
    std::vector<std::shared_ptr<Event>> memEvents;


};

#endif //GADGET_EVENTGENERATORPARAMETERS_H
