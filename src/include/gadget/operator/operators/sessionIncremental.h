
#ifndef GADGET_SESSIONINCREMENTAL_H
#define GADGET_SESSIONINCREMENTAL_H

// TODO (john): Move some of the following to the base classes
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "include/gadget/distributions/arrival.h"
#include "include/gadget/event/event.h"
#include "include/gadget/event/eventGenerator.h"
#include "include/gadget/distributions/key.h"
#include "include/gadget/operation.h"
#include "include/gadget/operator/operator.h"
#include "include/gadget/distributions/serviceTime.h"
#include "include/gadget/statemachine/stateMachines.h"
#include "include/gadget/distributions/value.h"
#include "include/gadget/distributions/windowLength.h"
#include "include/gadget/operator/operators/operatorParameters.h"

// TODO (john): Move type definitions to Utils
typedef uint64_t Timestamp;
typedef std::string KeyT;  // TODO (john): Rename this to 'Key' and change 'Key' to 'KeyDistribution'
typedef std::string ValueT;  // TODO (john): Rename this to 'Value' and change 'Value' type to 'ValueDistribution'


// TODO (john): Merge this with Operation in operation.h
class  StateOperation2 {

public:
    /***
     *
     * @param opType: The type of the operation (e.g. get, put, delete, ...)
     * @param time: The timestamp of the operation
     * @param key: The key to access state
     * @param value: The value associated with the key (can be empty, e.g., for get and delete operations)
     */
    explicit StateOperation2(Operation::OperationType opType,
                            Timestamp  time,
                            KeyT key,
                            ValueT value);
    /***
     *
     * Generates an empty state operation of the given type
     */
    explicit StateOperation2(Operation::OperationType opType);

    /***
     *
     * The operation's type
     */
    Operation::OperationType  type;
    /***
     *
     * The operation's timestamp
     */
    Timestamp  time;
    /***
     *
     * The key to access state
     */
    KeyT  key;
    /***
     *
     *  The value associate with the key (can be empty, e.g., for get and delete operations)
     */
    ValueT value;
    /***
     *
     *  @return the state operation as a string
     */
    std::string toString(bool=false);
};

class  SessionKeyedIncrementalStateMachine;



// TODO (john): Move this to 'WindowStateMachine'
class  ExpirationIndex2 {

public:
    explicit ExpirationIndex2(uint64_t lateness);

    /***
     *
     * The length of the allowed lateness in the session window operator (in event time)
     */
    uint64_t latenessLength;
    /***
     *
     * A mapping from event time to the list of state machines that expire at that time
     */
    std::map<uint64_t, std::list<std::shared_ptr<SessionKeyedIncrementalStateMachine>>> expiredStateMachines;
    /***
     * @param sm: The state machine to add to the index
     */
    void insert(std::shared_ptr<SessionKeyedIncrementalStateMachine> sm);
    /***
     * @param sm: The state machine to remove form the index
     */
    void remove(std::shared_ptr<SessionKeyedIncrementalStateMachine> sm);
    /***
     * @param watermark: The timestamp to search for expired state machines
     * @param stateMachinesToClose: The list of exprired state machines removed from the index
     */
    void collect(Timestamp watermark, std::list<std::shared_ptr<SessionKeyedIncrementalStateMachine>> &stateMachinesToClose);
    /***
     *
     * @param time: The time to search for state machines
     */
    uint64_t num(Timestamp time);
};

class SessionKeyedIncremental : public Operator,
                             public std::enable_shared_from_this<SessionKeyedIncremental> {

public:
    explicit SessionKeyedIncremental(std::shared_ptr<OperatorParameters> params);


    /***
     *
     * @param operationList  is the list of all state operations for the next batch of events
     * @return
     */

    bool runOperator(std::vector<std::shared_ptr<Operation>> &operationList) override;

    /***
     *
     * @return the last generated operation
     */
    std::shared_ptr<StateOperation2> nextOp();
    /***
     *
     * @return true if the operator has more events to process in the input buffer
     */
    bool pendingEvents();
    /***
     *
     * @return true if there was at least one session window merge
     */
    bool mappingsMerged();
    /**
     *
     * Assigns a state machine to the next input event and updates window mappings
     * NOTE (john): This method must be implemented by the user
     * TODO (john): Move to the parent 'WindowOperator'
     */
    void assignStateMachines();
    /**
     *
     * Generates all state operators triggered by the current event
     * NOTE (john): Must be called after assignStateMahcines()
     */
    void generateStateOperation2s();
    /**
     *
     * Closes all state machines whose windows have expired and generates the respective state operations
     * NOTE (john): This method must be implemented by the user
     * TODO (john): Move to the parent 'WindowOperator'
     */
    void closeExpiredStateMachines();
    /**
     *
     * Clears the state associated with the current event
     * NOTE (john): This method must be implemented by the user
     */
    void clearEventState();



private:
    /**
     *
     * A string whose size (in bytes) equals the size of a metadata entry
     */
    std::string metadataEntry;
    /***
     *
     * NOTE (john): Why is this needed?
     */
    int64_t  expectedNumOperation_;
    /***
     *
     * The total number of state operations generated so far
     */
    uint64_t totalNumOperations;
    /***
     *
     * NOTE (john): Not used
     */
    std::shared_ptr<Key> KeyDistribution_;
    /***
     *
     * The distribution of value sizes
     */
    std::shared_ptr<Value> ValueDistribution_;
    /***
     *
     * The last generated state operation
     */
    std::shared_ptr<StateOperation2> lastStateOperation2;
    /***
     *
     * The current active state machine in the operator
     */
    std::shared_ptr<SessionKeyedIncrementalStateMachine> currentActiveStateMachine;
    /***
     *
     * The input event generator
     */
    std::shared_ptr<EventGenerator> eventGenerator;
    /***
     *
     * The input buffer
     */
    std::vector<std::shared_ptr<Event>> inputBuffer;
    /***
     *
     * The last seen watermark
     * NOTE (john): We currently assume that watermarks arrive in order
     */
    uint64_t  lastWatermark;
    /***
     *
     * The previous watermark
     * TODO (john): Fix watermark generation so that we don't need this
     */
    uint64_t  prevWatermark;
    /***
     *
     * The index of the current event in the input buffer
     */
    uint64_t  eventIndex;
    /***
     *
     * The session gap
     */
    uint64_t  sessionGap;
    /***
     *
     *  The allowed lateness for out-of-order events
     */
    uint64_t  latenessLength;
    /***
     *
     * Indicates whether the operator accounts for out-of-order events
     */
    bool latenessAllowed;
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
     * The distribution of service times (used to generate transition delays in state machines)
     * TODO (john): We should have more than one distributions (one per type of operation)
     */
    std::shared_ptr<ServiceTime> serviceTimeDistribution;
    /***
     *
     * A mapping from event key to a list of state machines for that key (only one for session window)
     */
    std::unordered_map<std::string, std::list<std::shared_ptr<SessionKeyedIncrementalStateMachine>>> activeStateMachines;
    /***
     *
     * A mapping from event time to the list of state machines that expire at that time
     */
    std::shared_ptr<ExpirationIndex2> expiredStateMachines;
    /***
     *
     * The list of state machines assigned to the current event (only one for session window)
     */
    std::list<std::shared_ptr<SessionKeyedIncrementalStateMachine>> assignedStateMachines;
    /**
     *
     * A mapping from key=(start, end) to pairs of the form (winId, #elements)
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
     * A FIFO queue of state operations
     */
    std::queue<std::shared_ptr<StateOperation2>> operations;

    // NOTE (john): The associated state machine needs access to the queue of operations
    friend class SessionKeyedIncrementalStateMachine;
};


class  SessionKeyedIncrementalStateMachine {



    enum StatesType {
        GetState = 0, PutState, MergeState, FinalGetState, FinalDeleteState, ScanState, NextState, GetMetaState, MergeMetaState, DeleteMetaState,
        GetMetadataState,
        MergeMetadataState,
        DeleteMetadataState,
        LimboState
    };

public:
    /***
     *
     * @param currentKey: The key processed by the state machine
     * @param serviceTimeDistribution: A distribution of service times used to simulate the delay between state transitions
     * @param startTime: The start time of the window the state machine corresponds to
     * @param endTime: The end time of the window the state machine corresponds to
     * @param operatorInstance: The session window operator instance associated with the state machine
     */
    explicit SessionKeyedIncrementalStateMachine(std::string currentKey ,
                                                 std::shared_ptr<ServiceTime>  serviceTimeDistribution,
                                                 uint64_t startTime,
                                                 uint64_t endTime,
                                                 std::shared_ptr<SessionKeyedIncremental> operatorInstance);

    /***
     *
     * @return true if a state operation was generated, false otherwise
     * NOTE (john): This method must be implemented by the user
     * TODO (john): Rename to hasNext() and change method to return a pointer to the actual operation
     * object instead of a bool
     */
    bool HasNext();



    /***
     *
     * Executes the state machine until the final stte and generate the corresponding state operations.
     * Returns true if there was at least one generated state operation
     */
    bool run();
    /***
     *
     * @return the unique ID of the window the state machine corresponds to. This ID serves as the key
     * to access the window contents in the state store.
     * TODO (john): Move this to the parent 'WindowStateMachine'
     */
    std::string winId();
    /***
     *
     * @param event: The event to process
     * Resets the state machine to process the given event
     */
    void reset(std::shared_ptr<Event> event);
    /***
     *
     * @param newState: The new state
     * Sets the state of the state machine
     */
    void setState(StatesType newState);
    /***
     *
     * Transitions to the final state (called when the window expires)
     */
    void close();
    /***
     *
     * @return the start time of the window the state machine corresponds to
     * TODO (john): Move to the parent 'WindowStateMachine'
     */
    uint64_t  getStartTime();
    /***
     *
     * @param newStart: The new start time
     * Sets the start time of the window the state machine corresponds to
     * TODO (john): Move to the parent 'WindowStateMachine'
     */
    void  setStartTime(uint64_t newStart);
    /***
     *
     * @return the end time of the window the state machine corresponds to
     * TODO (john): Move to the parent 'WindowStateMachine'
     */
    uint64_t  getEndTime();
    /***
     *
     * @param newEnd: The new end time
     * Sets the end time of the window the state machine corresponds to
     * TODO (john): Move to the parent 'WindowStateMachine'
     */
    void  setEndTime(uint64_t newEnd);
    /**
     *
     * @return the key of the current event processed by the state machine
     * TODO (john): Move to the parent 'StateMachine'
     */
    std::string  getCurrentEventKey();

private:
    /**
     *
     * The metadata ID for the current event key
     */
    std::string metaKey;
    /***
     *
     * The session window operator instance associated with the state machine
     */
    std::shared_ptr<SessionKeyedIncremental>  opInstance;
    /***
     *
     * The total number of operations generated by the state machine so far
     */
    uint64_t  numOfDoneOperation;
    /**
     *
     * The start time of the window the state machine corresponds to
     */
    uint64_t  startTime;
    /**
     *
    * The end time of the window the state machine corresponds to
    */
    uint64_t  endTime;
    /**
     *
     * The unique ID of the window the state machine corresponds to
     */
    std::string currentKey;
    /**
     *
     * The current state
     * TODO (john): Rename 'StatesType' to 'Statetype'
     */
    StatesType state;
    /**
     *
     * isDone is true if the state machine is exhausted and false otherwise
     */
    bool isDone;
    /**
     *
     * The type of the last generated operation on state
     */
    Operation::OperationType currentOpr;
    /**
     *
     * The last generated operation on state
     */
    std::shared_ptr<StateOperation2> lastOperation;
    /**
     *
     * The transition delay from the current state to the next one
     */
    Timestamp  currentSleepPeriod;
    /***
     *
     * The distribution of service times (used to generate transition delays in state machines)
     * TODO (john): We should have more than one distributions (one per type of operation)
     */
    std::shared_ptr<ServiceTime>  serviceTimeDistribution;
    /**
     *
     * The current event processed by the state machine
     */
    std::shared_ptr<Event> currentEvent;
};


#endif //GADGET_SESSIONINCREMENTAL_H
