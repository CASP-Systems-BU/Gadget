//
// Created by Showan Asyabi on 4/3/21.
//

#ifndef GADGET_FILEWRAPPER_H
#define GADGET_FILEWRAPPER_H

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include  <filesystem>

#include "include/gadget/performanceMetrics.h"
#include "include/gadget/kvwrappers/kvwrapper.h"
typedef std::chrono::high_resolution_clock::time_point TimeVar;


class FileWrapper: public KVWrapper {
public:
    explicit FileWrapper(const std::string& path) {
        filePath_ = path;
        currentTime_ = 0;
    }

    bool connect() override {
        if(std::filesystem::exists(filePath_)) {
            gadgetFile_.open( filePath_+"/gadget.log" );
        } else {
            std::cout << "Please make the following file path: " <<  filePath_  << std::endl;
            return false;
        }

        std::cout << "The log file goes to : " << filePath_+"/gadget.log" << std::endl;
        return gadgetFile_.is_open();
    }

    bool disconnect() override {
        gadgetFile_.close();
        return true;
    }


    void dumpOperationsOnFile() override {
        assert(valuesList.size() > 0);
        assert(valuesList.size() == keysList.size());
        assert(valuesList.size() == operationsList.size());
        uint64_t counter = 0;

        while (counter <= valuesList.size()) {
            if(counter < sleepTimes.size()) {
                //currentTime_ += sleepTimes[counter];

            }
            currentTime_ += 1;
            counter ++;
            switch (operationsList[counter]) {
                case Operation::putOpr:
                    gadgetFile_<<currentTime_ <<" put " <<keysList[counter]<< " " << valuesList[counter] <<" gadget gadgetFile" << std::endl;
                    break;
                case Operation::getOpr:
                    gadgetFile_<< currentTime_<<" get " <<keysList[counter]<< " " << valuesList[counter] <<" gadget gadgetFile" << std::endl;
                    break;
                case Operation::mergeOpr:
                    gadgetFile_<<currentTime_ <<" merge " <<keysList[counter]<< " " << valuesList[counter] <<" gadget gadgetFile" << std::endl;
                    break;
                case Operation::deleteOpr:
                    gadgetFile_<< currentTime_<<" delete " <<keysList[counter]<< " " << valuesList[counter] <<" gadget gadgetFile" << std::endl;
                    break;
                case Operation::scanOpr:
                    gadgetFile_<< currentTime_<<" seek " <<keysList[counter]<< " " << valuesList[counter] <<" gadget gadgetFile" << std::endl;
                    break;
                case Operation::nextOpr:
                    gadgetFile_<< currentTime_<<" next " <<keysList[counter]<< " " << valuesList[counter] <<" gadget gadgetFile" << std::endl;
                    break;
            }
        }

    }


    void runBatchLatency() {
        // not defined
    }
    double runBatchThroughput() {
        // not defined
        return 0;
    }

private:

    std::string filePath_;
    std::ofstream gadgetFile_;
    uint64_t  currentTime_;

};


#endif //GADGET_FILEWRAPPER_H
