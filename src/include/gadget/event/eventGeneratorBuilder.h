//
// Created by Showan Asyabi on 4/13/21.
//

#ifndef GADGET_EVENTGENERATORBUILDER_H
#define GADGET_EVENTGENERATORBUILDER_H

#include "include/gadget/event/eventGenerator.h"
#include "include/gadget/event/eventGeneratorParameters.h"
#include "include/gadget/event/gadgetEventGenerator.h"
#include "include/gadget/event/traceEvents.h"
#include "include/gadget/event/memoryEvents.h"

class EventGeneratorBuilder {
public:
    static std::optional<std::shared_ptr<EventGenerator>> BuildEventGenerator(EventGenerator::EventGeneratorType egType,  std::shared_ptr<EventGeneratorParameters> params ) {
         if (egType == EventGenerator::Gadget) {
            return std::make_shared<GadgetEventGenerator>(params);
        } else if (egType == EventGenerator::TraceFile) {
            return std::make_shared<TraceEvents>(params);
        } else if (egType == EventGenerator::MemEvents) {
            return std::make_shared<MemoryEvents>(params);
        }
        return  {};
    }

};

#endif //GADGET_EVENTGENERATORBUILDER_H
