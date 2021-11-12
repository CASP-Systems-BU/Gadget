//
// Created by Showan Asyabi on 3/7/21.
//

#ifndef GADGET_EVENT_H
#define GADGET_EVENT_H
#include <string>
class Event {
public:

    Event() {
    }

    /***
     * @param eventTime  is time that event happened in  the source
     * @param arrivalTime_ is the inter arrival  time between this event and the last event . the time that event comes to the streaming system
     * @param key is the key of the event defined by the source of the event
     */
    Event(double eventTime, double arrivalTime,  std::string key) {
        eventTime_ =  eventTime;
        arrivalTime_ = arrivalTime;
        key_ = key;
        type_ = -1; // no type
    }

    /***
     * @param eventTime  is time that event happened in  the source
     * @param arrivalTime_ is the inter arrival  time between this event and the last event . the time that event comes to the streaming system
     * @param key is the key of the event defined by the source of the event
     * @param type is the type of the  the events. Type comes handy is some operators(e.g., continuous  join )
     */
    Event(double eventTime, double arrivalTime,  std::string key, std::string type) {
        eventTime_ =  eventTime;
        arrivalTime_ = arrivalTime;
        key_ = key;
        type_ = type; // no type
    }

    /***
     * eventTime_ The time the event happened in the source
     */
    double eventTime_;

    /***
     * arrivalTime_ is the time that event arrives to streaming system
     */
    double arrivalTime_;

    /***
     * key_ is the key of the event which is determined  by  the source
     */
    std::string key_;


    /***
     * some events might have a specific type that can be used by streaming systems
     * to process data stream
     */
    std::string type_;

};

#endif //GADGET_EVENT_H
