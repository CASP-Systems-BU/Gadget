
#ifndef GADGET_intervalJoinSM_H
#define GADGET_intervalJoinSM_H
#include "stateMachines.h"

class intervalJoinSM : public StateMachines {

public:
    enum intervalJoinStateType {
        DeleteFromLeftBuffer = 0, DeleteFromRightBuffer, GetRightBuffer , AddToRightBuffer,  ScanLeftBuffer, NextLeftBuffer, GetLeftBuffer ,AddToLeftBuffer, ScanRightBuffer, NextRightBuffer
    };

    intervalJoinSM(std::string Key, uint64_t lowerBound, uint64_t upperBound) {
        stateMachineKey_ = Key;
        lowerBound_ = lowerBound;
        upperBound_ = upperBound;
        isDone_ = false;
        currentEvent_ = nullptr;
        leftBufferPointer_ = 0;
        rightBufferPointer_ = 0;
        currentTime_ = 0;
        mainKey_ = Key; // fixme

    }

    void reset(std::shared_ptr<Event> currentEvent, bool leftStream = true, uint64_t waterMark = 1) override {
        currentEvent_ =  currentEvent;
        currentTime_ = currentEvent_->eventTime_;
        isDone_ = false;
        /***
         *We check if we can erase a portion of the buffer at this time
         *If not, we simply add the event to the buffer
         */

        if(leftStream) {
            if (cleanLeftBuffer.find(waterMark) != cleanLeftBuffer.end()) {
                State_ = DeleteFromLeftBuffer;
            } else {
                State_ = GetLeftBuffer;
            }
        } else {
            if(cleanRightBuffer.find(waterMark) != cleanRightBuffer.end()) {
                State_ = DeleteFromRightBuffer ;
            } else {
                State_ = GetRightBuffer ;
            }
        }
    }


    void closeStateMachine(bool leftStream = true) override {
        // not defined for cont join
    }

   /***
    * State machine for interval join is complicated. One should know
    * what is interval join before reading this code. We have two stream of events: left and right.
    * for each incoming event  we add the event to its corresponding buffer and schedule a delete time for this event.
    * In real streaming systems suh as Flink, we add the event to a buffer and check the other buffer to find the record
    * that can be joined.
    * @return
    */
    bool HasNext() override {
        if (isDone_ || (currentEvent_ == nullptr)) {
            return false;
        }
        switch (State_) {
            case DeleteFromLeftBuffer: {
                uint64_t time = (upperBound_ <= 0) ? currentTime_ : currentTime_ - upperBound_;
                leftBuffer_.erase(time);
                cleanLeftBuffer.erase(currentTime_);
                stateMachineKey_ = mainKey_ + std::to_string(time);
                currentOpr_ = Operation::deleteOpr;
                State_ = AddToLeftBuffer;
                isDone_ = false;
                break;
            }
            case DeleteFromRightBuffer: {
                uint64_t time = (lowerBound_ <= 0) ? currentTime_ : currentTime_ - upperBound_;
                rightBuffer_.erase(time);
                stateMachineKey_ = mainKey_ +std::to_string(time);
                cleanRightBuffer.erase(currentTime_);
                State_ = GetRightBuffer;
                currentOpr_ = Operation::deleteOpr;
                isDone_ = false;
                break;
            }
            case GetRightBuffer:
                State_ = AddToRightBuffer;
                currentOpr_ = Operation::getOpr;
                stateMachineKey_ = mainKey_ +std::to_string(currentTime_);
                isDone_ = false;
                break;
            case AddToRightBuffer: {
                State_ = ScanLeftBuffer;
                currentOpr_ = Operation::putOpr;
                if(rightBuffer_.find(currentTime_) != rightBuffer_.end()) {
                    rightBuffer_[currentTime_].push_back(currentEvent_);
                } else {
                    rightBuffer_.insert(std::pair<uint64_t, std::vector<std::shared_ptr<Event>>> (currentTime_, {currentEvent_}));
                }
                stateMachineKey_ = mainKey_ +std::to_string(currentTime_);
                uint64_t expireTime = (-lowerBound_ > 0) ? currentTime_ - lowerBound_ : currentTime_;
                cleanRightBuffer[expireTime]= 1; // 1 is a dummy value
                isDone_ = false;
                break;
            }
            case ScanLeftBuffer: {
                currentOpr_ = Operation::scanOpr;
                stateMachineKey_ = mainKey_;
                if (leftBuffer_.size() > 0) {
                    State_ = NextLeftBuffer;
                    isDone_ = false;
                    leftBufferPointer_ = 0;
                } else {
                    State_ = ScanLeftBuffer;
                    isDone_ = true;
                }
                break;
            }
            case NextLeftBuffer: {
                State_ = NextLeftBuffer;
                currentOpr_ = Operation::nextOpr;
                stateMachineKey_ = mainKey_;
                leftBufferPointer_++;
                if (leftBufferPointer_ >= leftBuffer_.size()) {
                    isDone_ = true;
                    leftBufferPointer_ = 0;
                } else {
                    isDone_ = false;
                }
                break;
            }
            case GetLeftBuffer: {
                State_ = AddToLeftBuffer;
                currentOpr_ = Operation::getOpr;
                stateMachineKey_ = mainKey_+std::to_string(currentTime_);
                isDone_ = false;
                State_ = AddToLeftBuffer;
                break;
            }
            case AddToLeftBuffer: {
                State_ = ScanRightBuffer;
                currentOpr_ = Operation::putOpr;
                leftBuffer_[currentTime_].push_back(currentEvent_);
                uint64_t expireTime = (upperBound_ > 0) ? currentTime_ + upperBound_ : currentTime_;
                stateMachineKey_ = mainKey_+std::to_string(currentTime_);
                cleanLeftBuffer[expireTime]= 1; // 1 is just a dummy value
                isDone_ = false;
                State_ = ScanRightBuffer;
                break;
            }
            case ScanRightBuffer: {
                currentOpr_ = Operation::scanOpr;
                stateMachineKey_ = mainKey_;
                if (rightBuffer_.size() > 0) {
                    isDone_ = false;
                    rightBufferPointer_ = 0;
                    State_ = NextRightBuffer;
                } else {
                    isDone_ = true;
                }
                break;
            }
            case NextRightBuffer: {
                rightBufferPointer_++;
                State_ = NextRightBuffer;
                currentOpr_ = Operation::nextOpr;
                stateMachineKey_ = mainKey_;
                rightBufferPointer_++;
                if (rightBufferPointer_ >= rightBuffer_.size()) {
                    isDone_ = true;
                    rightBufferPointer_ = 0;
                } else {
                    isDone_ = false;
                }
            }
        }
        return true;
    }

private:
    std::unordered_map<uint64_t, std::vector<std::shared_ptr<Event>>> leftBuffer_;
    std::unordered_map<uint64_t, std::vector<std::shared_ptr<Event>>> rightBuffer_;
    uint64_t  leftBufferPointer_;
    uint64_t  rightBufferPointer_;
    // cleanLeftBuffer holds all the scheduled clean times for left buffer
    std::unordered_map<uint64_t, uint64_t> cleanLeftBuffer;
    std::unordered_map<uint64_t, uint64_t> cleanRightBuffer;
    uint64_t  lowerBound_;
    uint64_t  upperBound_;
    intervalJoinStateType State_;
    uint64_t  currentTime_;
    // since in interval join keys are combination of machine key and timestamp we use this mainKey
    std::string mainKey_;
};



#endif //GADGET_INTERVALJOINSM_H
