#include "include/gadget/operator/operators/sessionHolistic.h"

// TODO (john): Merge this with Operation in operation.cpp
StateOperation::StateOperation(Operation::OperationType opType,
                               Timestamp  time,
                               KeyT key, ValueT value){
    type  = opType;
    this->time = time;
    this->key = key;
    this->value = value;
}

// Returns the state operation as a std::string
std::string StateOperation::toString(bool omit_value_size) {
    std::string s = std::string("");
    switch(type) {
        case Operation::getOpr:
            s += "get(";
            break;
        case Operation::deleteOpr:
            s += "delete(";
            break;
        case Operation::putOpr:
            s += "put(";
            break;
        case Operation::mergeOpr:
            s += "merge(";
            break;
        default :
            std::cerr << "Invalid operation type" << std::endl;
    }
    if (omit_value_size) {
        s += key + ")";
    }
    else{
        s += key + "), value_size=" + std::to_string(value.size());
    }
    return s;
}

// TODO (john): Move this to 'WindowStateMachine'
ExpirationIndex::ExpirationIndex(uint64_t lateness){
    latenessLength = lateness;
}

// TODO (john): Move this to 'WindowStateMachine'
// Inserts the given state machine into the expiration index
void ExpirationIndex::insert(std::shared_ptr<SessionKeyedHolisticStateMachine> sm) {
    // cout << "Inserting entry that expires at " << sm->getEndTime()+latenessLength << endl;
    // Index state machine according to the new end time
    expiredStateMachines[sm->getEndTime()+latenessLength].push_back(sm);
}

// TODO (john): Move this to 'WindowStateMachine'
// Removes the given machine from the expiration index
void ExpirationIndex::remove(std::shared_ptr<SessionKeyedHolisticStateMachine> sm) {
    auto entry = expiredStateMachines.find(sm->getEndTime()+latenessLength);
    assert(entry!=expiredStateMachines.end());
    auto sms = entry->second;  // A list of state machines that expire at the same time
    for (auto it=sms.begin(); it!=sms.end(); ++it) {
        if ((*it)->winId()==sm->winId()) {
            sms.erase(it);  // Remove state machine from the list
            if (sms.size()==0) {  // Delete entry if list is empty
                expiredStateMachines.erase(entry);
            }
            break;
        }
    }
}

// TODO (john): Move this to 'WindowStateMachine'
// Collects the expired state machines at the given watermark and removes them from the index
void ExpirationIndex::collect(Timestamp watermark,
                              std::list<std::shared_ptr<SessionKeyedHolisticStateMachine>> &stateMachinesToClose) {
    auto limit = expiredStateMachines.lower_bound(watermark);
    auto iter=expiredStateMachines.begin();
    while (iter!=limit) {
        // Add state machine to the list of state machines to close
        for (auto sm : iter->second) {
            stateMachinesToClose.push_back(sm);
        }
        // Clear entry
        iter->second.clear();
        iter = expiredStateMachines.erase(iter);
    }
}

// TODO (john): Move this to 'WindowStateMachine'
// Returns the number of state machines that expire at the given time
uint64_t ExpirationIndex::num(Timestamp time) {
    return expiredStateMachines[time+latenessLength].size();
}

SessionKeyedHolisticStateMachine::SessionKeyedHolisticStateMachine(std::string currentKey ,
                                                                   std::shared_ptr<ServiceTime>  serviceTimeDistribution,
                                                                   uint64_t startTime,
                                                                   uint64_t finishTime,
                                                                   std::shared_ptr<SessionKeyedHolistic> operatorInstance){ // fixme make std::string ref const
    this->currentKey = currentKey;
    this->startTime = startTime;
    this->endTime = finishTime;
    this->serviceTimeDistribution = serviceTimeDistribution;
    assert(this->serviceTimeDistribution);
    state = LimboState;
    isDone = false;
    currentSleepPeriod = 0;
    numOfDoneOperation = 0;
    currentEvent = nullptr;
    lastOperation = nullptr;
    opInstance = operatorInstance;
    assert(opInstance);
    metaKey = std::string("TBD");
}

// Resets the state machine for the current event
void SessionKeyedHolisticStateMachine::reset(std::shared_ptr<Event> currentEvent) {
    this->currentEvent = currentEvent;
    state = GetMetadataState;  // The next operation must fetch the metadata for the current event key
    metaKey = std::string("metadata_") + currentEvent->key_;  // The metadata key in the state store
    isDone = false;
    assert(opInstance);  // Make sure the state machine is associated with an operator instance
}

// Sets state machine as closed (i.e., the window has expired)
void SessionKeyedHolisticStateMachine::close() {
    isDone = false;  // There are pending operations to fetch and delete window contents and metadata
    state = FinalGetState; // Next operation must fetch the expired window contents
}

// Sets the state of the state machine
void SessionKeyedHolisticStateMachine::setState(StatesType newState) {
    state = newState;
}

// Returns the start time fo the window
uint64_t SessionKeyedHolisticStateMachine::getStartTime() {
    return  startTime;
}

// Sets the start time of the window
void SessionKeyedHolisticStateMachine::setStartTime(uint64_t newStart) {
    startTime = newStart;
}

// Returns the end time fo the window
uint64_t SessionKeyedHolisticStateMachine::getEndTime() {
    return  endTime;
}

// Sets the end time of the window
void SessionKeyedHolisticStateMachine::setEndTime(uint64_t newEnd) {
    endTime = newEnd;
}

// Returns the current event key
std::string SessionKeyedHolisticStateMachine::getCurrentEventKey() {
    return currentEvent->key_;
}

// Runs the state machine and generates the corresponding state access operations
bool SessionKeyedHolisticStateMachine::run() {
    if(isDone || !currentEvent) {
        return false;
    }
    assert((state==GetMetadataState) || (state==FinalGetState));
    // A pointer to the session window operator instance associated with the state machine
    auto op = std::dynamic_pointer_cast<SessionKeyedHolistic>(opInstance);
    assert(op);
    // TODO (john): Different state transitions should draw from different service time distributions
    if (state==GetMetadataState) {  // Step 1: Generate state operations triggered by the current event
        // Step 1.1: Generate operation that fetches the metadata for the current event key
        state = GetState;
        currentOpr = Operation::getOpr;
        lastOperation = std::make_shared<StateOperation>(StateOperation(currentOpr, currentSleepPeriod, metaKey, ""));
        op->operations.push(lastOperation);
        //currentSleepPeriod += serviceTimeDistribution->Next();
        numOfDoneOperation++;
        // TODO (john): This should be get-put in state stores that do not support merge
        // Step 1.2: Generate operation that merges current event to the assigned window in the state store
        currentOpr = Operation::mergeOpr;
        lastOperation = std::make_shared<StateOperation>(StateOperation(currentOpr, currentSleepPeriod, currentKey,
                                                                   op->ValueDistribution_->Next()));
        op->operations.push(lastOperation);
        //currentSleepPeriod += serviceTimeDistribution->Next();
        numOfDoneOperation++;
        // Step 1.3: Generate operations corresponding to state merges
        if (op->mergeDone) {
            for (auto entry : op->mappingsToDelete) {
                auto winId = entry.second.first;
                if (!winId.compare("None")) continue; // The current event was merged into an existing window
                // Step 1.3.1: Generate operation that fetches merged window contents
                auto numElements = entry.second.second;
                currentOpr = Operation::getOpr;
                lastOperation = std::make_shared<StateOperation>(StateOperation(currentOpr, currentSleepPeriod, winId, ""));
                op->operations.push(lastOperation);
                //currentSleepPeriod += serviceTimeDistribution->Next();
                numOfDoneOperation++;
                // Step 1.3.2: Generate operation that merges window contents with the contents of the new window
                currentOpr = Operation::mergeOpr;
                std::string v(numElements * op->ValueDistribution_->Next().size(), '0');  // A std::string of size equal to the size of the merged window contents
                lastOperation = std::make_shared<StateOperation>(StateOperation(currentOpr, currentSleepPeriod, currentKey, v));
                op->operations.push(lastOperation);
                //currentSleepPeriod += serviceTimeDistribution->Next();
                numOfDoneOperation++;
                // Step 1.3.3: Generate operation that deletes the merged window
                currentOpr = Operation::deleteOpr;
                lastOperation = std::make_shared<StateOperation>(StateOperation(currentOpr, currentSleepPeriod, winId, ""));
                op->operations.push(lastOperation);
                //currentSleepPeriod += serviceTimeDistribution->Next();
                numOfDoneOperation++;
            }
            // Step 1.3.4: Generate operation that deletes the metadata for the current key
            if (op->deleteMetadata) {
                currentOpr = Operation::deleteOpr;
                lastOperation = std::make_shared<StateOperation>(StateOperation(currentOpr, currentSleepPeriod, metaKey, ""));
                op->operations.push(lastOperation);
                //currentSleepPeriod += serviceTimeDistribution->Next();
                numOfDoneOperation++;
            }
            // else {
            //     cout << "Metadata were empty. No need to delete aything" << endl;
            // }
            // Step 1.3.5: Generate operation that inserts the updated metadata entries
            auto entries = &op->windowMappings[currentEvent->key_];

            assert(entries->size()!=0);

            for (auto entry : *entries) {
                currentOpr = Operation::mergeOpr;
                lastOperation = std::make_shared<StateOperation>(StateOperation(currentOpr, currentSleepPeriod, metaKey,
                                                                           op->metadataEntry));
                op->operations.push(lastOperation);
                //currentSleepPeriod += serviceTimeDistribution->Next();
                numOfDoneOperation++;
            }
        }
        else { // Step 1.4: No windows were merged. Generate operation that inserts the entry
            currentOpr = Operation::mergeOpr;
            lastOperation = std::make_shared<StateOperation>(StateOperation(currentOpr, currentSleepPeriod, metaKey,
                                                                       op->metadataEntry));
            op->operations.push(lastOperation);
            //currentSleepPeriod += serviceTimeDistribution->Next();
            numOfDoneOperation++;
        }
    }
    else if (state==FinalGetState) {  // Step 2: Generate state operations triggered by window expiration
        // Step 2.1: Generate operation that fetched the contents of the expired window
        currentOpr = Operation::getOpr;
        lastOperation = std::make_shared<StateOperation>(StateOperation(currentOpr, currentSleepPeriod, currentKey, ""));
        op->operations.push(lastOperation);
        //currentSleepPeriod += serviceTimeDistribution->Next();
        numOfDoneOperation++;
        // Step 2.2: Generate operation that deletes the expired window
        currentOpr = Operation::deleteOpr;
        lastOperation = std::make_shared<StateOperation>(StateOperation(currentOpr, currentSleepPeriod, currentKey, ""));
        op->operations.push(lastOperation);
        //currentSleepPeriod += serviceTimeDistribution->Next();
        numOfDoneOperation++;
        // Step 2.3: Generate operation that deletes metadata for the current key
        currentOpr = Operation::deleteOpr;
        lastOperation = std::make_shared<StateOperation>(StateOperation(currentOpr, currentSleepPeriod, metaKey, ""));
        op->operations.push(lastOperation);
        //currentSleepPeriod += serviceTimeDistribution->Next();
        numOfDoneOperation++;
        // Step 2.4: Generate operations that insert metadata entries for the current key
        // NOTE (john): Is this a single operation or multiple operations?
        auto entries = &op->windowMappings[currentEvent->key_];
        for (auto entry : *entries) {
            currentOpr = Operation::mergeOpr;
            lastOperation = std::make_shared<StateOperation>(StateOperation(currentOpr, currentSleepPeriod, metaKey,
                                                                       op->metadataEntry));
            op->operations.push(lastOperation);
            //currentSleepPeriod += serviceTimeDistribution->Next();
            numOfDoneOperation++;
        }
    }
    isDone = true;
    return isDone;
}

// NOTE (john): Not used
bool SessionKeyedHolisticStateMachine::HasNext() {
    std::cerr << "HasNext() not implemented for SessionKeyedHolisitcStateMachine!" << std::endl;
    return true;
}




// Returns the id of the state machine
std::string SessionKeyedHolisticStateMachine::winId() {
    return currentKey;
}



SessionKeyedHolistic::SessionKeyedHolistic(std::shared_ptr<OperatorParameters> params) {
    expiredStateMachines = std::make_shared<ExpirationIndex>(params->latenessLength);
    KeyDistribution_ = params->keysDistribution;
    ValueDistribution_ = params->valuesDistribution;
    this->serviceTimeDistribution = params->serviceTimeDistribution;
    this->sessionGap = params->windowLength; // we use window length for session gap
    this->latenessLength = params->latenessLength;
    this->latenessAllowed = params->allowedLateness;
    this->eventGenerator = params->eventGenerator;
    expectedNumOperation_ = params->expectedNumOperations;
    //isDone = false; //
    lastWatermark = 0;
    eventIndex = 0;
    totalNumOperations = 0;
    currentActiveStateMachine = nullptr;
    // TODO (john): Make this a parameter
    metadataEntry = "dummy_metadata_entry";
}

bool SessionKeyedHolistic::runOperator(std::vector<std::shared_ptr<Operation>> &operationList) {
   /* while (true) {
        // Step 1: Check if there are state operations in the queue
        if (!operations.empty()) {
            lastStateOperation = operations.front();
            // cout << lastStateOperation->toString() << endl;
            operations.pop();
            return true;
        }
        if (!pendingEvents()) {  // Input batch is exhausted
            // Step 2: Trigger all expired windows (if any) and push the respective state operations to the queue
            closeExpiredStateMachines();
            // Step 3: Pull the next batch of events from the input
            prevWatermark = lastWatermark;
            lastWatermark = eventGenerator->generateNextPane();
            // cout << "Last watermark: " << lastWatermark << endl;
            if (lastWatermark == 0) return false;  // Input stream is exhausted
            inputBuffer = eventGenerator->getPane(lastWatermark);
            eventIndex = 0;  // Reset the index of the current event in the batch
            // If there are no events in the next unit of event time try pulling the next batch
            if (inputBuffer.size() == 0) continue;
        }
        // Step 4: Assign state machines to the next event in the batch
        assignStateMachines();
        // Step 5: Generate all state operations triggered by the current event and push them to the queue
        generateStateOperations();
        // Clear operator state for next event
        clearEventState();
        eventIndex++;
    } */
         // get the next batch of events
        inputBuffer.clear();
        uint64_t  time = eventGenerator->getEventBatch(inputBuffer);
        if(time == 0)  { // this indicates there no more event to process
            return false;
        }

    // Step 2: Trigger all expired windows (if any) and push the respective state operations to the queue
        closeExpiredStateMachines();
        // Step 3: Pull the next batch of events from the input
        prevWatermark = lastWatermark;
        lastWatermark = eventGenerator->getWaterMark();
        eventGenerator->forgetPanes(time);
        // cout << "Last watermark: " << lastWatermark << endl;
        //if (lastWatermark == 0) return false;  // Input stream is exhausted
        //inputBuffer = eventGenerator->getPane(lastWatermark);
        eventIndex = 0;  // Reset the index of the current event in the batch
        //std::cout << "input buffer size " << inputBuffer.size() <<std::endl;

        while (eventIndex <  inputBuffer.size()) {

            assignStateMachines();
            // Step 5: Generate all state operations triggered by the current event and push them to the queue
            generateStateOperations();
            // Clear operator state for next event
            clearEventState();
            eventIndex++;
    }

    // return the operation

    if (!operations.empty()) {
        while (!operations.empty()) {
            lastStateOperation = operations.front();

            //std::cout << lastStateOperation->toString() << std::endl;
            auto opr = std::make_shared<Operation>(lastStateOperation->type, lastStateOperation->key,
                                                   lastStateOperation->value);
            operationList.push_back(opr);
            operations.pop();
        }
    }
   //std::cout << "operationList" << operationList.size() << std::endl;
    return true;

}





// Returns the last state operation performed
std::shared_ptr<StateOperation> SessionKeyedHolistic::nextOp() {
    return lastStateOperation;
}

// Returns true is there are more input events to process in the current batch, false otherwise
bool SessionKeyedHolistic::pendingEvents() {
    return inputBuffer.size() > eventIndex;
}

// Clears state of the current input event
void SessionKeyedHolistic::clearEventState() {
    assignedStateMachines.clear();
    mappingsToDelete.clear();
}

// Generates all state access operations
void SessionKeyedHolistic::generateStateOperations() {
    for (auto sm : assignedStateMachines) {
        sm->run();
    }
}

// Returns true if there was a session window merging due to the last processed event, false otherwise
bool SessionKeyedHolistic::mappingsMerged() {
    return mergeDone;
}

// Closes all state machines that have expired and generates the respective state operations
void SessionKeyedHolistic::closeExpiredStateMachines() {
    // Step 1: Fetch all expired state machines
    std::list<std::shared_ptr<SessionKeyedHolisticStateMachine>> stateMachinesToClose;
    expiredStateMachines->collect(lastWatermark, stateMachinesToClose);
    // cout << "Found " << stateMachinesToClose.size() << " state machines to close." << endl;
    // Step 2: Close state machines and generate the remaining state operations
    for (auto sm : stateMachinesToClose) {
        // cout << "Start: " << sm->getStartTime() << " End: " << sm->getEndTime() << " Watermark: " << lastWatermark << endl;
        // Step 2.1: Remove the entry that corresponds to the closed state machine from session window mappings
        bool found = false;
        for (auto entry = windowMappings.begin(); entry!=windowMappings.end(); ++entry) {
            for (auto it=entry->second.begin(); it!=entry->second.end(); ++it) {  // For all mappings per event key
                auto winId = it->second.first;
                if (winId==sm->winId()) {
                    entry->second.erase(it);
                    found = true;
                    break;
                }
            }
            if (found) break;  // Each state machine (window) corresponds to exacty one event key
        }
        found = false;
        // Step 2.2: Remove closed state machine from the list of active state machines
        for (auto entry = activeStateMachines.begin(); entry!=activeStateMachines.end(); ++entry) {
            for (auto it=entry->second.begin(); it!=entry->second.end(); ++it) {
                if ((*it)->winId()==sm->winId()) {
                    entry->second.erase(it);
                    found = true;
                    break;
                }
                if (found) break;
            }
        }
        // Step 2.3: Set final state and generate remaining operations
        sm->close();
        sm->run();
    }
    stateMachinesToClose.clear();
}

// Assigns a state machine to the next input event and updates operator state (incl. session window mappings)
void SessionKeyedHolistic::assignStateMachines() {
    // Make sure all mappings have been updated properly after processing the last event
    assert(mappingsToDelete.size()==0);
    assert(assignedStateMachines.size()==0);
    // The current input event
    auto event = inputBuffer[eventIndex];
     //std::cout << "Current event time " << event->eventTime_ << " and key " << event->key_ << " at watermark " << lastWatermark << std::endl;
    // Ignore too late events
    if ( (event->eventTime_ + latenessLength) < prevWatermark ) return;
    // Step 1: Fetch active state machines for the current event key
    auto stateMachinesForCurrentKey = &activeStateMachines[event->key_];
    deleteMetadata = stateMachinesForCurrentKey->size()>0;
    // The boundaries of the window the current event belongs to
    Timestamp windowStartTime = event->eventTime_,
            windowEndTime = event->eventTime_ + sessionGap;
    // Step 2: Find the window the event belongs to and do the necessary window mergings (if any)
    bool merged = false;
    for (auto sm : *stateMachinesForCurrentKey) {  // For each active state machine for the current key
        if (windowStartTime <= sm->getEndTime() &&
            windowEndTime >= sm->getStartTime()) {
            mergeDone = merged = true;
            // The current session id in mappings
            std::string sessionId = std::to_string(sm->getStartTime()) + "-" + std::to_string(sm->getEndTime());
            //std::cout << "Current session id: " << sessionId << std::endl;
            // Step 2.1: Assign state machine and window id to the current event
            auto mappings = &windowMappings[event->key_];
            auto mapping = &mappings->at(sessionId);
            assignedWindowForCurrentEvent = mapping->first;
            //std::cout << "New assigned window for current event: " << assignedWindowForCurrentEvent << std::endl;
            assignedStateMachines.push_back(sm);
            // Step 2.2: Update mapping entries to delete ("None" means that no state merging is needed)
            mappingsToDelete[sessionId] = std::pair<std::string, uint64_t>(std::string("None"), 0);
            // Step 2.3: Update expiration dictionary
            expiredStateMachines->remove(sm);
            sm->setStartTime(std::min(windowStartTime, sm->getStartTime()));
            sm->setEndTime(std::max(windowEndTime, sm->getEndTime()));
            expiredStateMachines->insert(sm);
            // Step 2.4: Update session window mappings
            std::string newSessionId = std::to_string(sm->getStartTime()) + "-" + std::to_string(sm->getEndTime());
            //std::cout << "New session id: " << newSessionId << std::endl;
            mapping->second += 1;  // Count the current event
            mappings->erase(sessionId);
            mappings->insert(std::make_pair(newSessionId, *mapping));
            // showan: I changed the order the above two lines - The old order caused problems

        }
        if (merged) break;
    }
    if (!merged) {
        // Step 2: Create a new state machine for the current event and update operator state
        auto new_sm = std::make_shared<SessionKeyedHolisticStateMachine>(KeyDistribution_->Next(),
                                                                         serviceTimeDistribution,
                                                                         windowStartTime,
                                                                         windowEndTime,
                                                                         shared_from_this());


        new_sm->reset(event);
        // Step 3.1: Assign state machine and window id to the current event
        stateMachinesForCurrentKey->push_back(new_sm);  // Update active state machines for the current key
        assignedStateMachines.push_back(new_sm);  // Update assigned state machines for the current event
        assignedWindowForCurrentEvent = new_sm->winId();
        // Step 3.2: Update expiration dictionary
        expiredStateMachines->insert(new_sm);
        // Step 3.3: Update session window mappings
        std::string sessionId = std::to_string(new_sm->getStartTime()) + "-" + std::to_string(new_sm->getEndTime());
        auto mappings = &windowMappings[event->key_];
        mappings->insert(std::make_pair(sessionId, std::pair<std::string, uint64_t>(new_sm->winId(), 1)));
        return;
    }
    do {  // Step 4: Check if there are more windows to merge and update state accordingly
        merged = false;
        //std::cout << "Number of state machines for current key: " << stateMachinesForCurrentKey->size() << std::endl;
        for (auto i=stateMachinesForCurrentKey->begin(); i!=stateMachinesForCurrentKey->end(); ++i) {
            auto stateMachine1 = *i;
            for (auto j=next(i,1); j!=stateMachinesForCurrentKey->end(); ++j) {
                auto stateMachine2 = *j;
                // Check if the two windows should be merged
                if (stateMachine2->getStartTime() <= stateMachine1->getEndTime() &&
                    stateMachine2->getEndTime() >= stateMachine1->getStartTime()) {
                    mergeDone = merged = true;
                    // Let's keep the state machine with the earliest start time and discard the other one
                    auto stateMachineToKeep = (stateMachine1->getStartTime() < stateMachine2->getStartTime() ? stateMachine1 : stateMachine2);
                    auto stateMachineToDelete = (stateMachineToKeep->winId() == stateMachine1->winId() ? stateMachine2 : stateMachine1);
                    auto sessionIdToDelete1 = std::to_string(stateMachineToKeep->getStartTime()) + "-" + std::to_string(stateMachineToKeep->getEndTime());
                    auto sessionIdToDelete2 = std::to_string(stateMachineToDelete->getStartTime()) + "-" + std::to_string(stateMachineToDelete->getEndTime());
                    auto winToKeep = stateMachineToKeep->winId();
                    //std::cout << "Win id to keep: " << winToKeep << std::endl;
                    auto winToDelete = stateMachineToDelete->winId();
                     //std::cout << "Win id to delete: " << winToDelete << std::endl;
                    auto newEnd = std::max(stateMachine1->getEndTime(), stateMachine2->getEndTime());
                    auto mappings = &windowMappings[event->key_];
                    auto mKeep = mappings->at(sessionIdToDelete1);
                    auto mDelete = mappings->at(sessionIdToDelete2);
                    // Step 4.1: Re-assign state machine and window id to the current event
                    if (winToDelete.compare(assignedWindowForCurrentEvent)==0) {
                        assignedWindowForCurrentEvent = winToKeep;
                        assignedStateMachines.remove(stateMachineToDelete);
                        assignedStateMachines.push_back(stateMachineToKeep);
                    }
                    // Step 4.2: Remove old state machine from the list of state machines for the current key
                    stateMachinesForCurrentKey->remove(stateMachineToDelete);
                    // Step 4.3: Update mapping entries to delete
                    mappingsToDelete[sessionIdToDelete2] = std::pair<std::string, uint64_t>(winToDelete, mDelete.second);
                    // Step 4.4: Update expiration dictionary
                    expiredStateMachines->remove(stateMachineToDelete);
                    expiredStateMachines->remove(stateMachineToKeep);
                    stateMachineToKeep->setEndTime(newEnd);
                    expiredStateMachines->insert(stateMachineToKeep);
                    // Step 4.5: Update session window mappings
                    std::string newSessionId = std::to_string(stateMachineToKeep->getStartTime()) + "-" + std::to_string(stateMachineToKeep->getEndTime());
                    mKeep.second += mDelete.second;  // Count the merged events
                    mappings->insert(std::make_pair(newSessionId, mKeep));
                    mappings->erase(sessionIdToDelete1);
                    mappings->erase(sessionIdToDelete2);
                    break;
                }
            }
            if (merged) break;
        }
    } while (merged);
    assert(assignedStateMachines.size()==1);  // There must be only one assigned state machine
    assignedStateMachines.front()->reset(event);
}
