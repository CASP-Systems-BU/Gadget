//
// Created by Showan Asyabi on 4/13/21.
//

#ifndef GADGET_EVENTGENERATOR_H
#define GADGET_EVENTGENERATOR_H
#include <memory>
#include "event.h"


class EventGenerator {
public:
    enum  EventGeneratorType : uint16_t {
        Gadget, TraceFile, MemEvents
    };
  virtual ~EventGenerator() = default;
  virtual  uint64_t generateNextPane_outOfOrder() = 0;
  virtual  uint64_t generateNextPane() = 0;
  virtual  std::vector<std::shared_ptr<Event>> getPane(uint64_t watermark) = 0;
  virtual bool  forgetPanes(uint64_t watermark) = 0;
  virtual void printPane(uint64_t watermark)  = 0;
  virtual uint64_t getWaterMark() = 0 ;
  virtual uint64_t getEventBatch(std::vector<std::shared_ptr<Event>> &eventBatch) = 0;
};

#endif //GADGET_EVENTGENERATOR_H
