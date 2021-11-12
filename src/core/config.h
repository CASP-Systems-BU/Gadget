//
// Created by Showan Asyabi on 3/28/21.
//
#include <iostream>
#include <memory>
#include <fstream>
#include <optional>

#include "gadget/distributions/arrival.h"
#include "gadget/distributions/key.h"
#include "gadget/distributions/value.h"
#include "gadget/distributions/windowLength.h"
#include "gadget/kvwrappers/kvwrapper.h"
#include "gadget/kvwrappers/wrapperBuilder.h"
#include "gadget/performanceMetrics.h"
#include "gadget/distributions/distributionParamters.h"
#include "include/gadget/operator/operators/operatorParameters.h"
#include "gadget/operator/operator.h"


/***
 * This class build all distributions and  the operators
 * This class first read the config file into a dictionary
 * if  it finds  a config in the  config file it uses those configs; otherwise, it uses the
 * default values
 */

class Config {
public:
    Config(const std::string& pathToConfigFile) {
        // read the config file and make a dictionary of user settings
        std::ifstream fileStream(pathToConfigFile);
        if(!fileStream) {
            std::cout << "Error: The config file does not exist" << std::endl;
            throw std::exception();
        }

        std::string line;

        while (std::getline(fileStream, line))
        {   // remove spaces from the line
            line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
            // convert std::string to lower case
            std::transform(line.begin(), line.end(), line.begin(),
                           [](unsigned char c){ return std::tolower(c); });

            std::istringstream is_line(line);
            std::string key;
            if (std::getline(is_line, key, '='))
            {
                std::string value;
                if (key[0] == '#')
                    continue;

                if (std::getline(is_line, value))
                {
                    configValues[key] = value;
                }
            }
        }
        fileStream.close();
        std::cout << "The config file has been successfully  read! " <<std::endl;

    }

///////////////////////////////////////////////////////////////
//// Make the needed distributions for the first event generator
////////////////////////////////////////////////////////////////

    std::optional<std::shared_ptr<KeyPopularity>>  firstEventGeneratorKeyPopularityDistrib() {
        if (configValues.find("first.event.generator.key.popularity.distrib.type") != configValues.end()) {
            // first lets find the distribution type
           KeyPopularity::KeyPopularityDistribs keyPopularityType;
            bool distribIsRecognized = false;
            // check if the user have provided  all the needed parameters for his/her needed distribution; if not, print error and exit
            if(configValues["first.event.generator.key.popularity.distrib.type"] == "constant") {
                keyPopularityType =  KeyPopularity::ConstantDistrib;
                distribIsRecognized = true;
            } else if(configValues["first.event.generator.key.popularity.distrib.type"] == "hotspot") {
                keyPopularityType =  KeyPopularity::hotSpotDistrib;
                distribIsRecognized = true;
            } else if(configValues["first.event.generator.key.popularity.distrib.type"] == "zipf") {
                keyPopularityType =  KeyPopularity::ZipfDistrib;
                distribIsRecognized = true;
            } else if(configValues["first.event.generator.key.popularity.distrib.type"] == "sequential") {
                keyPopularityType =  KeyPopularity::TemporalIncreasing;
                distribIsRecognized = true;
            } else if(configValues["first.event.generator.key.popularity.distrib.type"] == "uniform") {
                keyPopularityType =  KeyPopularity::UniformIntDistrib;
                distribIsRecognized = true;
            }

            uint64_t  keySpaceSize = 0;
            if (configValues.find("first.event.generator.key.popularity.distrib.params.keyspacesize") != configValues.end()) {
                try {
                    keySpaceSize = stoi(configValues["first.event.generator.key.popularity.distrib.params.keyspacesize"]);
                } catch (...) {
                    std::cout<<"Error:  firstEventGenerator&KeyPopularity: " << "Need  a number for  keyspacesize" << std::endl;
                    return {};
                }
            } else {
                std::cout<<"Error:  firstEventGenerator&KeyPopularity: " << "The  key space size ('first.event.generator.key.popularity.distrib.params.keyspacesize') is not defined" << std::endl;
                return {};
            }

            // if the distribution type is not recognized, return
            if( distribIsRecognized != true) {
                std::cout<<"Error:  firstEventGenerator&KeyPopularity:" << "The distribution type ('first.event.generator.key.popularity.distrib.type') is not recognized" << std::endl;
                return {};
            }

            // make the distribution parameters
            auto  distributionParameters = std::make_shared<DistributionParameters>();


            switch (keyPopularityType) {
                case KeyPopularity::ConstantDistrib: // ConstInt is needed
                    if(configValues.find("first.event.generator.key.popularity.distrib.params.constant") != configValues.end()) {
                        try {
                            distributionParameters->constantString = configValues["first.event.generator.key.popularity.distrib.params.constant"];
                        } catch (...) {
                            std::cout<<"Error:  firstEventGenerator&KeyPopularity: " << "Need a string for 'first.event.generator.key.popularity.distrib.params.constant'" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  firstEventGenerator&KeyPopularity: " << "please define 'first.event.generator.key.popularity.distrib.params.constant'" << std::endl;
                        return {};
                    }
                    break;
                case KeyPopularity::hotSpotDistrib: // hotspotfraction and hotoperationfraction are needed
                    if(configValues.find("first.event.generator.key.popularity.distrib.params.hotspotfraction") != configValues.end()) {
                        try {
                            distributionParameters->hotSpotFraction = stod(configValues["first.event.generator.key.popularity.distrib.params.hotspotfraction"]);
                        } catch (...) {
                            std::cout<<"Error:  firstEventGenerator&KeyPopularity: " << "KeyPopularity - HotSpotFraction ('first.event.generator.key.popularity.distrib.params.hotspotfraction')  must be a real number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  firstEventGenerator&KeyPopularity: " << "KeyPopularity - hotspotfraction ('first.event.generator.key.popularity.distrib.params.hotspotfraction') must be defined" << std::endl;
                        return {};
                    }
                    if(configValues.find("first.event.generator.key.popularity.distrib.params.hotoperationfraction") != configValues.end()) {
                        try {
                            distributionParameters->hotSpotFraction = stod(configValues["first.event.generator.key.popularity.distrib.params.hotoperationfraction"]);
                        } catch (...) {
                            std::cout<<"Error:  firstEventGenerator&KeyPopularity: "<< "KeyPopularity - hotoperationfraction ('first.event.generator.key.popularity.distrib.params.hotoperationfraction')  must be a real number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  firstEventGenerator&KeyPopularity: "<< "KeyPopularity - hotoperationfraction ('first.event.generator.key.popularity.distrib.params.hotoperationfraction') must be defined" << std::endl;
                        return {};
                    }
                    break;
                case KeyPopularity::ZipfDistrib: // s is needed
                    if(configValues.find("first.event.generator.key.popularity.distrib.params.s") != configValues.end()) {
                        try {
                            distributionParameters->s = stod(configValues["first.event.generator.key.popularity.distrib.params.s"]);
                        } catch (...) {
                            std::cout<<"Error:  firstEventGenerator&KeyPopularity: " << "The s parameter (first.event.generator.key.popularity.distrib.params.s)  must be a  real number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  firstEventGenerator&KeyPopularity: " << "The s parameter ('first.event.generator.key.popularity.distrib.params.s') must be defined" << std::endl;
                        return {};
                    }
                    break;
                case KeyPopularity::TemporalIncreasing: // no need for any paramater
                    break;

                case KeyPopularity::UniformIntDistrib: // need a and b
                    if(configValues.find("first.event.generator.key.popularity.distrib.params.a") != configValues.end()) {
                        try {
                            distributionParameters->a_real = stod(configValues["first.event.generator.key.popularity.distrib.params.a"]);
                        } catch (...) {
                            std::cout<<"Error:  firstEventGenerator&KeyPopularity: " << "KeyPopularity- The a parameter (first.event.generator.key.popularity.distrib.params.a)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  firstEventGenerator&KeyPopularity: " << "The a parameter (first.event.generator.key.popularity.distrib.params.a) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("first.event.generator.key.popularity.distrib.params.b") != configValues.end()) {
                        try {
                            distributionParameters->b_real = stod(configValues["first.event.generator.key.popularity.distrib.params.b"]);
                        } catch (...) {
                            std::cout<<"Error:  firstEventGenerator&KeyPopularity: "<< "The b parameter (first.event.generator.key.popularity.distrib.params.b)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  firstEventGenerator&KeyPopularity: " << "The b parameter ('first.event.generator.key.popularity.distrib.params.b') must be defined" << std::endl;
                        return {};
                    }
                    break;


            }
            // if the code is here all the distrib type and the needed params are ok- make the distribution
            return  KeyPopularityBuilder::BuildKeyPopularity(keyPopularityType, distributionParameters, keySpaceSize).value();
        } else { // the user does not have any config for this distribution.
            std::cout<<"Error:  firstEventGenerator&KeyPopularity: " << " The key popularity type ('first.event.generator.key.popularity.distrib.type') for first event generator must be defined" << std::endl;
            return {};
        }
    }

    std::optional<std::shared_ptr<Arrival>>  firstEventGeneratorEventOccurrenceDistrib() {
        if (configValues.find("first.event.generator.event.occurrence.gap.distrib.type") != configValues.end()) {
            // first lets find the distribution tyep
            Arrival::ArrivalDistribs ArrivalType;
            bool distribIsRecognized =false;
            // check if the user have provided  all the needed parameters for his/her needed distribution; if not, print error and exist
            if(configValues["first.event.generator.event.occurrence.gap.distrib.type"] == "exponential") {
                ArrivalType =  Arrival::ExponentialDistrib;
                distribIsRecognized = true;
            } else if(configValues["first.event.generator.event.occurrence.gap.distrib.type"] == "constant") {
                ArrivalType =  Arrival::ConstantDistrib;
                distribIsRecognized = true;
            } else if(configValues["first.event.generator.event.occurrence.gap.distrib.type"] == "uniform") {
                ArrivalType =  Arrival::UniformRealDistrib;
                distribIsRecognized = true;
            } else if(configValues["first.event.generator.event.occurrence.gap.distrib.type"] == "normal") {
                ArrivalType =  Arrival::NormalDistrib;
                distribIsRecognized = true;
            }

            // if the distribution type is not recognized, return
            if( distribIsRecognized != true) {
                std::cout<<"Error: first.event.generator&EventOccurrenceDistrib:" << "The distribution type (first.event.generator.event.occurrence.gap.distrib.type) is not recognized" << std::endl;
                return {};
            }

            // make the distribution parameters
            auto  distributionParameters = std::make_shared<DistributionParameters>();


            switch (ArrivalType) {
                case Arrival::ExponentialDistrib: // lambda is needed
                    if(configValues.find("first.event.generator.event.occurrence.gap.distrib.params.lambda") != configValues.end()) {
                        try {
                            distributionParameters->lambda = stod(configValues["first.event.generator.event.occurrence.gap.distrib.params.lambda"]);
                        } catch (...) {
                            std::cout<<"Error: first.event.generator&EventOccurrenceDistrib: " << "Lambda ('first.event.generator.event.occurrence.gap.distrib.params.lambda') must be a real number!" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: first.event.generator&EventOccurrenceDistrib:" << "parameter Lambda ('first.event.generator.event.occurrence.gap.distrib.params.lambda') must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Arrival::ConstantDistrib: // ConstInt is needed
                    if(configValues.find("first.event.generator.event.occurrence.gap.distrib.params.constant") != configValues.end()) {
                        try {
                            distributionParameters->constantReal = stod(configValues["first.event.generator.event.occurrence.gap.distrib.params.constant"]);
                        } catch (...) {
                            std::cout<<"Error: first.event.generator&EventOccurrenceDistrib: " << "The constant ('first.event.generator.event.occurrence.gap.distrib.params.constant')  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: first.event.generator&EventOccurrenceDistrib: " << "The constant (first.event.generator.event.occurrence.gap.distrib.params.constant) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Arrival::NormalDistrib: // sd an mu is needed
                    if(configValues.find("first.event.generator.event.occurrence.gap.distrib.params.sd") != configValues.end()) {
                        try {
                            distributionParameters->sd = stod(configValues["first.event.generator.event.occurrence.gap.distrib.params.sd"]);
                        } catch (...) {
                            std::cout<<"Error: first.event.generator&EventOccurrenceDistrib: " << "The sd parameter ('first.event.generator.event.occurrence.gap.distrib.params.sd')  must be a  real number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: first.event.generator&EventOccurrenceDistrib: " << "The sd parameter (first.event.generator.event.occurrence.gap.distrib.params.sd)  must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("first.event.generator.event.occurrence.gap.distrib.params.mean") != configValues.end()) {
                        try {
                            distributionParameters->mean = stod(configValues["first.event.generator.event.occurrence.gap.distrib.params.mean"]);
                        } catch (...) {
                            std::cout<<"Error: first.event.generator&EventOccurrenceDistrib: " << "The mean parameter ('first.event.generator.event.occurrence.gap.distrib.params.mean') must be a real number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: first.event.generator&EventOccurrenceDistrib: " << "The mean parameter ('first.event.generator.event.occurrence.gap.distrib.params.mean') must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Arrival::UniformRealDistrib: // a and b are needed
                    if(configValues.find("first.event.generator.event.occurrence.gap.distrib.params.a") != configValues.end()) {
                        try {
                            distributionParameters->a_real = stod(configValues["first.event.generator.event.occurrence.gap.distrib.params.a"]);
                        } catch (...) {
                            std::cout<<"Error: first.event.generator&EventOccurrenceDistrib: " << "The a parameter (first.event.generator.event.occurrence.gap.distrib.params.a)  must be a real number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: first.event.generator&EventOccurrenceDistrib: " << "The a parameter (first.event.generator.event.occurrence.gap.distrib.params.a) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("first.event.generator.event.occurrence.gap.distrib.params.b") != configValues.end()) {
                        try {
                            distributionParameters->b_real = stod(configValues["first.event.generator.event.occurrence.gap.distrib.params.b"]);
                        } catch (...) {
                            std::cout<<"Error: first.event.generator&EventOccurrenceDistrib: " << "The b parameter (first.event.generator.event.occurrence.gap.distrib.params.b)  must be a real number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: first.event.generator&EventOccurrenceDistrib: " << "The b parameter (first.event.generator.event.occurrence.gap.distrib.params.b) must be defined" << std::endl;
                        return {};
                    }

            }
            // if the code is here all the distrib type and the needed params are ok- make the distribution
            return ArrivalBuilder::BuildArrival(ArrivalType, distributionParameters).value();

        } else { // the user does not have any config for this distribution. =
            std::cout<<"Error: first.event.generator&EventOccurrenceDistrib: " << " The event occurrence distribution (first.event.generator.event.occurrence.gap.distrib.type) must be defined" << std::endl;
            return {};
        }
    }

    std::optional<std::shared_ptr<Arrival>>  firstEventGeneratorEventArrivalDistrib() {
        if (configValues.find("first.event.generator.arrival.interarrival.distrib.type") != configValues.end()) {
            // first lets find the distribution tyep
            Arrival::ArrivalDistribs ArrivalType;
            bool distribIsRecognized =false;
            // check if the user have provided  all the needed parameters for his/her needed distribution; if not, print error and exist
            if(configValues["first.event.generator.arrival.interarrival.distrib.type"] == "exponential") {
                ArrivalType =  Arrival::ExponentialDistrib;
                distribIsRecognized = true;
            } else if(configValues["first.event.generator.arrival.interarrival.distrib.type"] == "constant") {
                ArrivalType =  Arrival::ConstantDistrib;
                distribIsRecognized = true;
            } else if(configValues["first.event.generator.arrival.interarrival.distrib.type"] == "uniform") {
                ArrivalType =  Arrival::UniformRealDistrib;
                distribIsRecognized = true;
            } else if(configValues["first.event.generator.arrival.interarrival.distrib.type"] == "normal") {
                ArrivalType =  Arrival::NormalDistrib;
                distribIsRecognized = true;
            }

            // if the distribution type is not recognized, return
            if( distribIsRecognized != true) {
                std::cout<<"Error: first.event.generator&InterArrivalDistrib:" << "The distribution type (first.event.generator.arrival.interarrival.distrib.type) is not recognized" << std::endl;
                return {};
            }

            // make the distribution parameters
            auto  distributionParameters = std::make_shared<DistributionParameters>();


            switch (ArrivalType) {
                case Arrival::ExponentialDistrib: // lambda is needed
                    if(configValues.find("first.event.generator.arrival.interarrival.distrib.params.lambda") != configValues.end()) {
                        try {
                            distributionParameters->lambda = stod(configValues["first.event.generator.arrival.interarrival.distrib.params.lambda"]);
                        } catch (...) {
                            std::cout<<"Error: first.event.generator&InterArrivalDistrib: " << "Lambda (first.event.generator.arrival.interarrival.distrib.params.lambda) must be a  realnumber!" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: first.event.generator&InterArrivalDistrib:" << "parameter Lambda (first.event.generator.arrival.interarrival.distrib.params.lambda) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Arrival::ConstantDistrib: // ConstInt is needed
                    if(configValues.find("first.event.generator.arrival.interarrival.distrib.params.constant") != configValues.end()) {
                        try {
                            distributionParameters->constantReal = stod(configValues["first.event.generator.arrival.interarrival.distrib.params.constant"]);
                        } catch (...) {
                            std::cout<<"Error: first.event.generator&InterArrivalDistrib: " << "The constant (first.event.generator.arrival.interarrival.distrib.params.constant)  must be a real number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: first.event.generator&InterArrivalDistrib: " << "The constant (first.event.generator.arrival.interarrival.distrib.params.constant) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Arrival::NormalDistrib: // sd an mu is needed
                    if(configValues.find("first.event.generator.arrival.interarrival.distrib.params.sd") != configValues.end()) {
                        try {
                            distributionParameters->sd = stod(configValues["first.event.generator.arrival.interarrival.distrib.params.sd"]);
                        } catch (...) {
                            std::cout<<"Error: first.event.generator&InterArrivalDistrib: " << "The sd parameter (first.event.generator.arrival.interarrival.distrib.params.sd)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: first.event.generator&InterArrivalDistrib: " << "The sd parameter (first.event.generator.arrival.interarrival.distrib.params.sd) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("first.event.generator.arrival.interarrival.distrib.params.mean") != configValues.end()) {
                        try {
                            distributionParameters->mean = stod(configValues["first.event.generator.arrival.interarrival.distrib.params.mean"]);
                        } catch (...) {
                            std::cout<<"Error: first.event.generator&InterArrivalDistrib: " << "The mean parameter (first.event.generator.arrival.interarrival.distrib.params.mean)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: first.event.generator&InterArrivalDistrib: " << "The mean parameter (first.event.generator.arrival.interarrival.distrib.params.mean) must be defined" << std::endl;
                        return {};
                    }
                    break;

                case Arrival::UniformRealDistrib: // a and b are needed
                    if(configValues.find("first.event.generator.arrival.interarrival.distrib.params.a") != configValues.end()) {
                        try {
                            distributionParameters->a_real = stod(configValues["first.event.generator.arrival.interarrival.distrib.params.a"]);
                        } catch (...) {
                            std::cout<<"Error: first.event.generator&InterArrivalDistrib: " << "The a parameter (first.event.generator.arrival.interarrival.distrib.params.a)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: first.event.generator&InterArrivalDistrib: " << "The a parameter (first.event.generator.arrival.interarrival.distrib.params.a) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("first.event.generator.arrival.interarrival.distrib.params.b") != configValues.end()) {
                        try {
                            distributionParameters->b_real = stod(configValues["first.event.generator.arrival.interarrival.distrib.params.b"]);
                        } catch (...) {
                            std::cout<<"Error: first.event.generator&InterArrivalDistrib: " << "The b parameter (first.event.generator.arrival.interarrival.distrib.params.b)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: first.event.generator&InterArrivalDistrib: " << "The b parameter (first.event.generator.arrival.interarrival.distrib.params.b) must be defined" << std::endl;
                        return {};
                    }

            }
            // if the code is here all the distrib type and the needed params are ok- make the distribution
            return ArrivalBuilder::BuildArrival(ArrivalType, distributionParameters).value();

        } else { // the user does not have any config for this distribution.
            std::cout<<"Error: first.event.generator&InterArrivalDistrib: " << " The InterArrivalDistrib (first.event.generator.arrival.interarrival.distrib.type) must be defined" << std::endl;
            return {};
        }
    }

    /***
     * this function builds the first event generator
     * Currently, we support two types of event generators: 1) events from a file. For example, events from Flink traces  2)  Gadget event generator.
     * gadget event generator generates events based one several parameters
     *
     */
    std::optional<std::shared_ptr<EventGenerator>> buildFirstEventGenerator() {
        auto eventGeneratorParams = std::make_shared<EventGeneratorParameters>();

        // check the event generator type


        if (configValues.find("first.event.generator.type") != configValues.end()) {
            if (configValues.find("first.event.generator.watermark.frequency") != configValues.end()) {
                try {
                    eventGeneratorParams->waterMarkFrequency = stoi(configValues["first.event.generator.watermark.frequency"]);
                } catch (...) {
                    std::cout << "Error:  first.event.generator& watermark frequency " << " the  watermark frequency (first.event.generator.watermark.frequency) must be an integer number"
                              << std::endl;
                    return {};
                }
            } else {
                std::cout<<"Error:  firstEventGenerator& watermark frequency  : " << "the  watermark frequency (first.event.generator.watermark.frequency)  must be defined" << std::endl;
                return {};
            }

            if(configValues["first.event.generator.type"] == "tracefile") {

                // find the key index in the  event file
                if(configValues.find("first.event.generator.key.index") != configValues.end() ) {
                    try {
                        eventGeneratorParams->keyIndex = stoi(configValues["first.event.generator.key.index"]);
                    } catch (...) {
                        std::cout << "Error:  first.event.generator&key index " << " the key index (first.event.generator.key.index) must be a number"
                                  << std::endl;
                        return {};
                    }
                } else {
                    std::cout<<"Error:  firstEventGenerator&key index: " << "The  the key index (first.event.generator.key.index) (i.e, column number of the key - the number starts from zero) must be defined" << std::endl;
                    return {};
                }
                //find the event time index in the  event file
                if(configValues.find("first.event.generator.event.time.index") != configValues.end() ) {
                    try {
                        eventGeneratorParams->eventTimeIndex = stoi(
                                configValues["first.event.generator.event.time.index"]);
                    } catch (...) {
                        std::cout << "Error:  first.event.generator&event time index (first.event.generator.event.time.index) " << " The event time index (first.event.generator.event.time.index)  must be a number"
                                  << std::endl;
                        return {};
                    }
                } else {
                    std::cout<<"Error:  firstEventGenerator & event time  index: " << "The  the event time index (first.event.generator.event.time.index) (i.e, column number of the even time - the number starts from zero) must be defined" << std::endl;
                }


                //find the event type index in the  event file
                if(configValues.find("first.event.generator.event.type.index") != configValues.end() ) {
                    try {
                        eventGeneratorParams->typeIndex = stoi(
                                configValues["first.event.generator.event.type.index"]);
                    } catch (...) {
                        std::cout << "Error:  first.event.generator & type index " << " The type index  (first.event.generator.event.type.index) must be a number"
                                  << std::endl;
                        return {};
                    }
                } else {
                    std::cout<<"Warning:  no event  type index  is defined for the first event generator  " << " if the type  of events is not important, ignore this message; otherwise, define:  first.event.generator.event.type.index " << std::endl;
                }

                //find  the event file path
                if(configValues.find("first.event.generator.file.path") != configValues.end() ) {
                    try {
                        eventGeneratorParams->eventFilePath = configValues["first.event.generator.file.path"];
                    } catch (...) {
                        std::cout << "Error:  first.event.generator & file path " << " please define the file path including the file name (e.g., first.event.generator.file.path=config.txt)" <<std::endl;
                        return {};
                    }
                } else {
                    std::cout << "Error: no file path is defined for the first event  generator "
                              << "please define the file path including the file name (e.g., first.event.generator.file.path=config.txt)"
                              << std::endl;
                }
                // here we have all the needed information for an event generator; therefore, we can define the the first event generator

                auto eventGen  =  EventGeneratorBuilder::BuildEventGenerator(EventGenerator::TraceFile , eventGeneratorParams);
                return eventGen;

            } else if (configValues["first.event.generator.type"] == "gadget") {
                // we first make the needed distributions
                eventGeneratorParams->keyPopularity =  firstEventGeneratorKeyPopularityDistrib().value();
                eventGeneratorParams->eventOccurrenceTimeDistribution =   firstEventGeneratorEventOccurrenceDistrib().value();
                eventGeneratorParams->arrivalTimesDistribution =  firstEventGeneratorEventArrivalDistrib().value();


                eventGeneratorParams->outOfOrderPercentage = 0;
                eventGeneratorParams->latenessThreshold = 0;
                if(configValues.find("first.event.generator.outoforder.percentage") != configValues.end() ) {
                    try {
                        eventGeneratorParams->outOfOrderPercentage = stod(configValues["first.event.generator.outoforder.percentage"]);
                    } catch (...) {
                        std::cout << "Error:  first.event.generator & outoforder percentage " << " first.event.generator.outoforder.percentage must be a real number"
                                  << std::endl;
                        return {};
                    }
                } else {
                    // no need for error. It is  optional
                }

                if(configValues.find("first.event.generator.lateness.threshold") != configValues.end() ) {
                    try {
                        eventGeneratorParams->outOfOrderPercentage = stoi(configValues["first.event.generator.lateness.threshold"]);
                    } catch (...) {
                        std::cout << "Error:  first.event.generator & lateness threshold " << " first.event.generator.lateness.threshold  must be an integer number"
                                  << std::endl;
                        return {};
                    }
                } else {
                    // no need for error. It is  optional
                }

                auto eventGen  =  EventGeneratorBuilder::BuildEventGenerator(EventGenerator::Gadget, eventGeneratorParams);
                return eventGen;
            } else {
                std::cout<<"Error:  firstEventGenerator& type: " << " The type of first event generator (e.g., main, file) must be defined" << std::endl;
                return {};
            }
        } else { // event generator type is not defined
            std::cout<<"Error:  firstEventGenerator&type: " << " The type of first event generator (e.g., first.event.generator.type="
                                                               ""
                                                               "tracefile) must be defined" << std::endl;
            return  {};
        }
    }

////////////////////////////////////////////////////////////////
//// Make the needed distributions for the second event generator
/////////////////////////////////////////////////////////////////

    std::optional<std::shared_ptr<KeyPopularity>>  secondEventGeneratorKeyPopularityDistrib() {
        if (configValues.find("second.event.generator.key.popularity.distrib.type") != configValues.end()) {
            KeyPopularity::KeyPopularityDistribs keyPopularityType;
            bool distribIsRecognized =false;
            // check if the user have provided  all the needed parameters for his/her needed distribution; if not, print error and exist
            if(configValues["second.event.generator.key.popularity.distrib.type"] == "constant") {
                keyPopularityType =  KeyPopularity::ConstantDistrib;
                distribIsRecognized = true;
            } else if(configValues["second.event.generator.key.popularity.distrib.type"] == "hotspot") {
                keyPopularityType =  KeyPopularity::hotSpotDistrib;
                distribIsRecognized = true;
            } else if(configValues["second.event.generator.key.popularity.distrib.type"] == "zipf") {
                keyPopularityType =  KeyPopularity::ZipfDistrib;
                distribIsRecognized = true;
            } else if(configValues["second.event.generator.key.popularity.distrib.type"] == "sequential") {
                keyPopularityType =  KeyPopularity::TemporalIncreasing;
                distribIsRecognized = true;
            } else if(configValues["second.event.generator.key.popularity.distrib.type"] == "uniform") {
                keyPopularityType =  KeyPopularity::UniformIntDistrib;
                distribIsRecognized = true;
            }

            uint64_t  keySpaceSize = 0;
            if (configValues.find("second.event.generator.key.popularity.distrib.params.keyspacesize") != configValues.end()) {
                try {
                    keySpaceSize = stoi(configValues["second.event.generator.key.popularity.distrib.params.keyspacesize"]);
                } catch (...) {
                    std::cout<<"Error:  secondEventGenerator&KeyPopularity: " << "Need  a number for  keyspacesize" << std::endl;
                    return {};
                }
            } else {
                std::cout<<"Error:  secondEventGenerator&KeyPopularity: " << "The  key space size (second.event.generator.key.popularity.distrib.params.keyspacesize) is not defined" << std::endl;
                return {};
            }

            // if the distribution type is not recognized
            if( distribIsRecognized != true) {
                std::cout<<"Error:  secondEventGenerator&KeyPopularity:" << "The distribution type (second.event.generator.key.popularity.distrib.type) is not recognized" << std::endl;
                return {};
            }

            // make the distribution parameters
            auto  distributionParameters = std::make_shared<DistributionParameters>();


            switch (keyPopularityType) {
                case KeyPopularity::ConstantDistrib: // ConstInt is needed
                    if(configValues.find("second.event.generator.key.popularity.distrib.params.constant") != configValues.end()) {
                        try {
                            distributionParameters->constantString = configValues["second.event.generator.key.popularity.distrib.params.constant"];
                        } catch (...) {
                            std::cout<<"Error:  secondEventGenerator&KeyPopularity: " << "Need  a constant string for key popularity distribution (second.event.generator.key.popularity.distrib.params.constant)" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  secondEventGenerator&KeyPopularity: " << " The constant key (second.event.generator.key.popularity.distrib.params.constant) must be defined " << std::endl;
                        return {};
                    }
                    break;
                case KeyPopularity::hotSpotDistrib: // hotspotfraction and hotoperationfraction are needed
                    if(configValues.find("second.event.generator.key.popularity.distrib.params.hotspotfraction") != configValues.end()) {
                        try {
                            distributionParameters->hotSpotFraction = stod(configValues["second.event.generator.key.popularity.distrib.params.hotspotfraction"]);
                        } catch (...) {
                            std::cout<<"Error:  secondEventGenerator&KeyPopularity: " << "KeyPopularity - HotSpotFraction (second.event.generator.key.popularity.distrib.params.hotspotfraction)  must be a real number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  secondEventGenerator&KeyPopularity: " << "KeyPopularity - hotspotfraction (second.event.generator.key.popularity.distrib.params.hotspotfraction) must be defined" << std::endl;
                        return {};
                    }
                    if(configValues.find("second.event.generator.key.popularity.distrib.params.hotoperationfraction") != configValues.end()) {
                        try {
                            distributionParameters->hotSpotFraction = stod(configValues["second.event.generator.key.popularity.distrib.params.hotoperationfraction"]);
                        } catch (...) {
                            std::cout<<"Error:  secondEventGenerator&KeyPopularity: "<< "KeyPopularity - hotoperationfraction (second.event.generator.key.popularity.distrib.params.hotoperationfraction)  must be a real number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  secondEventGenerator&KeyPopularity: "<< "KeyPopularity - hotoperationfraction must be defined" << std::endl;
                        return {};
                    }
                    break;
                case KeyPopularity::ZipfDistrib: // s is needed
                    if(configValues.find("second.event.generator.key.popularity.distrib.params.s") != configValues.end()) {
                        try {
                            distributionParameters->s = stod(configValues["second.event.generator.key.popularity.distrib.params.s"]);
                        } catch (...) {
                            std::cout<<"Error:  secondEventGenerator&KeyPopularity: " << "The s parameter (second.event.generator.key.popularity.distrib.params.s)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  secondEventGenerator&KeyPopularity: " << "The s parameter (second.event.generator.key.popularity.distrib.params.s) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case KeyPopularity::TemporalIncreasing: // no need for any parameter
                    break;

                case KeyPopularity::UniformIntDistrib: // need a and b
                    if(configValues.find("second.event.generator.key.popularity.distrib.params.a") != configValues.end()) {
                        try {
                            distributionParameters->a_real = stod(configValues["second.event.generator.key.popularity.distrib.params.a"]);
                        } catch (...) {
                            std::cout<<"Error:  secondEventGenerator&KeyPopularity: " << "KeyPopularity- The a parameter  (second.event.generator.key.popularity.distrib.params.a) must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  secondEventGenerator&KeyPopularity: " << "The a parameter (second.event.generator.key.popularity.distrib.params.a) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("second.event.generator.key.popularity.distrib.params.b") != configValues.end()) {
                        try {
                            distributionParameters->b_real = stod(configValues["second.event.generator.key.popularity.distrib.params.b"]);
                        } catch (...) {
                            std::cout<<"Error:  secondEventGenerator&KeyPopularity: "<< "The b parameter (second.event.generator.key.popularity.distrib.params.b)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  secondEventGenerator&KeyPopularity: " << "The b parameter (second.event.generator.key.popularity.distrib.params.b) must be defined" << std::endl;
                        return {};
                    }
                    break;
            }
            // if the code is here all the distrib type and the needed params are ok- make the distribution
            return KeyPopularityBuilder::BuildKeyPopularity(keyPopularityType, distributionParameters, keySpaceSize).value();

        } else {
            std::cout<<"Error:  secondEventGenerator&KeyPopularity: " << " The 'second.event.generator.key.popularity.distrib.type' must be defined" << std::endl;
           return {};
        }
    }

    std::optional<std::shared_ptr<Arrival>>  secondEventGeneratorEventOccurrenceDistrib()   {
        if (configValues.find("second.event.generator.event.occurrence.gap.distrib.type") != configValues.end()) {
            //  lets find the distribution tyep
            Arrival::ArrivalDistribs ArrivalType;
            bool distribIsRecognized = false;
            // check if the user have provided  all the needed parameters for his/her needed distribution; if not, print error and exist
            if(configValues["second.event.generator.event.occurrence.gap.distrib.type"] == "exponential") {
                ArrivalType =  Arrival::ExponentialDistrib;
                distribIsRecognized = true;
            } else if(configValues["second.event.generator.event.occurrence.gap.distrib.type"] == "constant") {
                ArrivalType =  Arrival::ConstantDistrib;
                distribIsRecognized = true;
            } else if(configValues["second.event.generator.event.occurrence.gap.distrib.type"] == "uniform") {
                ArrivalType =  Arrival::UniformRealDistrib;
                distribIsRecognized = true;
            } else if(configValues["second.event.generator.event.occurrence.gap.distrib.type"] == "normal") {
                ArrivalType =  Arrival::NormalDistrib;
                distribIsRecognized = true;
            }

            // if the distribution type is not recognized, return
            if( distribIsRecognized != true) {
                std::cout<<"Error: second.event.generator&RateDistrib:" << "The distribution type (second.event.generator.event.occurrence.gap.distrib.type) is not recognized" << std::endl;
                return {};
            }

            // make the distribution parameters
            auto  distributionParameters = std::make_shared<DistributionParameters>();


            switch (ArrivalType) {
                case Arrival::ExponentialDistrib: // lambda is needed
                    if(configValues.find("second.event.generator.event.occurrence.gap.distrib.params.lambda") != configValues.end()) {
                        try {
                            distributionParameters->lambda = stod(configValues["second.event.generator.event.occurrence.gap.distrib.params.lambda"]);
                        } catch (...) {
                            std::cout<<"Error: second.event.generator&RateDistrib: " << "Lambda  (second.event.generator.event.occurrence.gap.distrib.params.lambda) must be a number!" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: second.event.generator&RateDistrib:" << "parameter Lambda (second.event.generator.event.occurrence.gap.distrib.params.lambda) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Arrival::ConstantDistrib: // ConstInt is needed
                    if(configValues.find("second.event.generator.event.occurrence.gap.distrib.params.constant") != configValues.end()) {
                        try {
                            distributionParameters->constantReal = stod(configValues["second.event.generator.event.occurrence.gap.distrib.params.constant"]);
                        } catch (...) {
                            std::cout<<"Error: second.event.generator&RateDistrib: " << "The constant (second.event.generator.event.occurrence.gap.distrib.params.constant)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: second.event.generator&RateDistrib: " << "The constant (second.event.generator.event.occurrence.gap.distrib.params.constant) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Arrival::NormalDistrib: // sd an mu is needed
                    if(configValues.find("second.event.generator.event.occurrence.gap.distrib.params.sd") != configValues.end()) {
                        try {
                            distributionParameters->sd = stod(configValues["second.event.generator.event.occurrence.gap.distrib.params.sd"]);
                        } catch (...) {
                            std::cout<<"Error: second.event.generator&RateDistrib: " << "The sd parameter (second.event.generator.event.occurrence.gap.distrib.params.sd)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: second.event.generator&RateDistrib: " << "The sd parameter (second.event.generator.event.occurrence.gap.distrib.params.sd) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("second.event.generator.event.occurrence.gap.distrib.params.mean") != configValues.end()) {
                        try {
                            distributionParameters->mean = stod(configValues["second.event.generator.event.occurrence.gap.distrib.params.mean"]);
                        } catch (...) {
                            std::cout<<"Error: second.event.generator&RateDistrib: " << "The mean parameter (second.event.generator.event.occurrence.gap.distrib.params.mean)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: second.event.generator&RateDistrib: " << "The mean parameter (second.event.generator.event.occurrence.gap.distrib.params.mean) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Arrival::UniformRealDistrib: // a and b are needed
                    if(configValues.find("second.event.generator.event.occurrence.gap.distrib.params.a") != configValues.end()) {
                        try {
                            distributionParameters->a_real = stod(configValues["second.event.generator.event.occurrence.gap.distrib.params.a"]);
                        } catch (...) {
                            std::cout<<"Error: second.event.generator&RateDistrib: " << "The a parameter (second.event.generator.event.occurrence.gap.distrib.params.a)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: second.event.generator&RateDistrib: " << "The a parameter  (second.event.generator.event.occurrence.gap.distrib.params.a) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("second.event.generator.event.occurrence.gap.distrib.params.b") != configValues.end()) {
                        try {
                            distributionParameters->b_real = stod(configValues["second.event.generator.event.occurrence.gap.distrib.params.b"]);
                        } catch (...) {
                            std::cout<<"Error: second.event.generator&RateDistrib: " << "The b  parameter (second.event.generator.event.occurrence.gap.distrib.params.b)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: second.event.generator&RateDistrib: " << "The b  parameter (second.event.generator.event.occurrence.gap.distrib.params.b) must be defined" << std::endl;
                        return {};
                    }

            }
            // if the code is here all the distrib type and the needed params are ok- make the distribution
            if( ArrivalBuilder::BuildArrival(ArrivalType, distributionParameters).has_value()) {
                return ArrivalBuilder::BuildArrival(ArrivalType, distributionParameters).value();
            } else {
                return {};
            }
        } else {std::cout<<"Error: second.event.generator&RateDistrib: " << "The 'second.event.generator.event.occurrence.gap.distrib.type' must be defined" << std::endl;
           return {};
        }
    }

    std::optional<std::shared_ptr<Arrival>> secondEventGeneratorEventArrivalDistrib() {
        if (configValues.find("second.event.generator.arrival.interarrival.distrib.type") != configValues.end()) {
            // second lets find the distribution tyep
            Arrival::ArrivalDistribs ArrivalType;
            bool distribIsRecognized =false;
            // check if the user have provided  all the needed parameters for his/her needed distribution; if not, print error and exist
            if(configValues["second.event.generator.arrival.interarrival.distrib.type"] == "exponential") {
                ArrivalType =  Arrival::ExponentialDistrib;
                distribIsRecognized = true;
            } else if(configValues["second.event.generator.arrival.interarrival.distrib.type"] == "constant") {
                ArrivalType =  Arrival::ConstantDistrib;
                distribIsRecognized = true;
            } else if(configValues["second.event.generator.arrival.interarrival.distrib.type"] == "uniform") {
                ArrivalType =  Arrival::UniformRealDistrib;
                distribIsRecognized = true;
            } else if(configValues["second.event.generator.arrival.interarrival.distrib.type"] == "normal") {
                ArrivalType =  Arrival::NormalDistrib;
                distribIsRecognized = true;
            }

            // if the distribution type is not recognized, return
            if( distribIsRecognized != true) {
                std::cout<<"Error: second.event.generator&RateDistrib:" << "The distribution type (second.event.generator.arrival.interarrival.distrib.type) is not recognized" << std::endl;
                return {};
            }

            // make the distribution parameters
            auto  distributionParameters = std::make_shared<DistributionParameters>();


            switch (ArrivalType) {
                case Arrival::ExponentialDistrib: // lambda is needed
                    if(configValues.find("second.event.generator.arrival.interarrival.distrib.params.lambda") != configValues.end()) {
                        try {
                            distributionParameters->lambda = stod(configValues["second.event.generator.arrival.interarrival.distrib.params.lambda"]);
                        } catch (...) {
                            std::cout<<"Error: second.event.generator&RateDistrib: " << "Lambda (second.event.generator.arrival.interarrival.distrib.params.lambda) must be a number!" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: second.event.generator&RateDistrib:" << "parameter Lambda (second.event.generator.arrival.interarrival.distrib.params.lambda) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Arrival::ConstantDistrib: // ConstInt is needed
                    if(configValues.find("second.event.generator.arrival.interarrival.distrib.params.constant") != configValues.end()) {
                        try {
                            distributionParameters->constantReal = stod(configValues["second.event.generator.arrival.interarrival.distrib.params.constant"]);
                        } catch (...) {
                            std::cout<<"Error: second.event.generator&RateDistrib: " << "The constant (second.event.generator.arrival.interarrival.distrib.params.constant)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: second.event.generator&RateDistrib: " << "The constant must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Arrival::NormalDistrib: // sd an mu is needed
                    if(configValues.find("second.event.generator.arrival.interarrival.distrib.params.sd") != configValues.end()) {
                        try {
                            distributionParameters->sd = stod(configValues["second.event.generator.arrival.interarrival.distrib.params.sd"]);
                        } catch (...) {
                            std::cout<<"Error: second.event.generator&RateDistrib: " << "The sd parameter (second.event.generator.arrival.interarrival.distrib.params.sd)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: second.event.generator&RateDistrib: " << "The sd parameter (second.event.generator.arrival.interarrival.distrib.params.sd) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("second.event.generator.arrival.interarrival.distrib.params.mean") != configValues.end()) {
                        try {
                            distributionParameters->mean = stod(configValues["second.event.generator.arrival.interarrival.distrib.params.mean"]);
                        } catch (...) {
                            std::cout<<"Error: second.event.generator&RateDistrib: " << "The mean parameter (second.event.generator.arrival.interarrival.distrib.params.mean)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: second.event.generator&RateDistrib: " << "The mean parameter (second.event.generator.arrival.interarrival.distrib.params.mean) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Arrival::UniformRealDistrib: // a and b are needed
                    if(configValues.find("second.event.generator.arrival.interarrival.distrib.params.a") != configValues.end()) {
                        try {
                            distributionParameters->a_real = stod(configValues["second.event.generator.arrival.interarrival.distrib.params.a"]);
                        } catch (...) {
                            std::cout<<"Error: second.event.generator&RateDistrib: " << "The a parameter (second.event.generator.arrival.interarrival.distrib.params.a)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: second.event.generator&RateDistrib: " << "The a parameter (second.event.generator.arrival.interarrival.distrib.params.a) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("second.event.generator.arrival.interarrival.distrib.params.b") != configValues.end()) {
                        try {
                            distributionParameters->b_real = stod(configValues["second.event.generator.arrival.interarrival.distrib.params.b"]);
                        } catch (...) {
                            std::cout<<"Error: second.event.generator&RateDistrib: " << "The b parameter (second.event.generator.arrival.interarrival.distrib.params.b)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: second.event.generator&RateDistrib: " << "The b parameter (second.event.generator.arrival.interarrival.distrib.params.b) must be defined" << std::endl;
                        return {};
                    }

            }
            // if the code is here all the distrib type and the needed params are ok- make the distribution
            return ArrivalBuilder::BuildArrival(ArrivalType, distributionParameters).value();

        } else { // the user does not have any config for this distribution.

            std::cout<<"Error: second.event.generator&RateDistrib: " << "The 'second.event.generator.arrival.interarrival.distrib.type' must be defined" << std::endl;
          return  {};
        }
    }

    /***
     * this function builds the second event generator
     */
    std::optional<std::shared_ptr<EventGenerator>> buildSecondEventGenerator() {
        auto eventGeneratorParams = std::make_shared<EventGeneratorParameters>();

        // check the event generator type
        if (configValues.find("second.event.generator.type") != configValues.end()) {

            if (configValues.find("second.event.generator.watermark.frequency") != configValues.end()) {
                try {
                    eventGeneratorParams->waterMarkFrequency = stoi(configValues["second.event.generator.watermark.frequency"]);
                } catch (...) {
                    std::cout << "Error:  second.event.generator& watermark frequency " << " the  watermark frequency (second.event.generator.watermark.frequency) must be an integer number"
                              << std::endl;
                    return {};
                }
            } else {
                std::cout<<"Error:  secondEventGenerator& watermark frequency  : " << "the  watermark frequency (second.event.generator.watermark.frequency)  must be defined" << std::endl;
                return {};
            }


            if(configValues["second.event.generator.type"] == "tracefile") {

                // find the key index in the  event file
                if(configValues.find("second.event.generator.key.index") != configValues.end() ) {
                    try {
                        eventGeneratorParams->keyIndex = stoi(configValues["second.event.generator.key.index"]);
                    } catch (...) {
                        std::cout << "Error:  second.event.generator&key index " << " the key index (second.event.generator.key.index) must be a number"
                                  << std::endl;
                        return {};
                    }
                } else {
                    std::cout<<"Error:  secondEventGenerator&key index: " << "The  the key index (second.event.generator.key.index) (i.e, column number of the key - the number starts from zero) must be defined" << std::endl;
                    return {};
                }
                //find the event time index in the  event file
                if(configValues.find("second.event.generator.event.time.index") != configValues.end() ) {
                    try {
                        eventGeneratorParams->eventTimeIndex = stoi(
                                configValues["second.event.generator.event.time.index"]);
                    } catch (...) {
                        std::cout << "Error:  second.event.generator&event time index " << " The event time index  (second.event.generator.event.time.index) must be a number"
                                  << std::endl;
                        return {};
                    }
                } else {
                    std::cout<<"Error:  secondEventGenerator & event time  index: " << "The  the event time index (second.event.generator.event.time.index) (i.e, column number of the even time - the number starts from zero) must be defined" << std::endl;
                }


                //find the event type index in the  event file
                if(configValues.find("second.event.generator.event.type.index") != configValues.end() ) {
                    try {
                        eventGeneratorParams->typeIndex = stoi(
                                configValues["second.event.generator.event.type.index"]);
                    } catch (...) {
                        std::cout << "Error:  second.event.generator & type index  ()" << " The type index (second.event.generator.event.type.index) must be a number"
                                  << std::endl;
                        return {};
                    }
                } else {
                    std::cout<<"Warning:  no event  type index  is defined for the second event generator  " << " if the type  of events is not important, ignore this message; otherwise, define:  second.event.generator.event.type.index " << std::endl;
                }

                //find  the event file path
                if(configValues.find("second.event.generator.file.path") != configValues.end() ) {
                    try {
                        eventGeneratorParams->eventFilePath = configValues["second.event.generator.file.path"];
                    } catch (...) {
                        std::cout << "Error:  second.event.generator & file path " << " please define the file path including the file name (e.g., second.event.generator.file.path=file.txt)"
                                  << std::endl;
                        return {};
                    }
                } else {
                    std::cout<<"Error: no file path is defined for the second event  generator   " << " please define the file path including the file name (e.g., second.event.generator.file.path=file.txt) " << std::endl;
                }


                // here we have all the needed information for an event generator; therefore, we can define the the second event generator

                auto eventGen  =  EventGeneratorBuilder::BuildEventGenerator(EventGenerator::TraceFile, eventGeneratorParams);
                return eventGen;

            } else if (configValues["second.event.generator.type"] == "gadget") {


                eventGeneratorParams->outOfOrderPercentage = 0;
                eventGeneratorParams->latenessThreshold = 0;
                if(configValues.find("second.event.generator.outoforder.percentage") != configValues.end() ) {
                    try {
                        eventGeneratorParams->outOfOrderPercentage = stod(configValues["second.event.generator.outoforder.percentage"]);
                    } catch (...) {
                        std::cout << "Error:  second.event.generator & outoforder percentage " << " second.event.generator.outoforder.percentage must be a real number"
                                  << std::endl;
                        return {};
                    }
                } else {
                   // no need for error. It is  optional
                }

                if(configValues.find("second.event.generator.lateness.threshold") != configValues.end() ) {
                    try {
                        eventGeneratorParams->outOfOrderPercentage = stoi(configValues["second.event.generator.lateness.threshold"]);
                    } catch (...) {
                        std::cout << "Error:  second.event.generator & lateness threshold " << " second.event.generator.lateness.threshold  must be an integer number"
                                  << std::endl;
                        return {};
                    }
                } else {
                    // no need for error. It is  optional
                }



                eventGeneratorParams->keyPopularity =  secondEventGeneratorKeyPopularityDistrib().value();
                eventGeneratorParams->eventOccurrenceTimeDistribution =   secondEventGeneratorEventOccurrenceDistrib().value();
                eventGeneratorParams->arrivalTimesDistribution =  secondEventGeneratorEventArrivalDistrib().value();

                auto eventGen  =  EventGeneratorBuilder::BuildEventGenerator(EventGenerator::Gadget, eventGeneratorParams);
                return eventGen;
            } else {
                std::cout<<"Error:  secondEventGenerator& type: " << " The type of second event generator (second.event.generator.type) (e.g., gadget, tracefile) must be defined" << std::endl;
            }
        } else {
            std::cout<<"Error:  secondEventGenerator& type: " << " The type of second event generator (second.event.generator.type) ((e.g., gadget, tracefile)  must be defined. Please add 'second.event.generator.type'" << std::endl;
            return  {};
        }
    }



/////////////////////////////////////////////////////////////
//// Make the needed distributions for operator
//////////////////////////////////////////////////////////////

    std::optional<std::shared_ptr<KeyPopularity>>  operatorKeyPopularityDistrib() {


        // make the key popularity distribution-  these keys are window keys, when a window is created the key of the new window is is created by this distribution
        if (configValues.find("operator.key.popularity.distrib.type") != configValues.end()) {
            // first lets find the distribution tyep
            KeyPopularity::KeyPopularityDistribs keyPopularityType;
            bool distribIsRecognized =false;
            // check if the user have provided  all the needed parameters for his/her needed distribution; if not, print error and exist
            if(configValues["operator.key.popularity.distrib.type"] == "constant") {
                keyPopularityType =  KeyPopularity::ConstantDistrib;
                distribIsRecognized = true;
            } else if(configValues["operator.key.popularity.distrib.type"] == "hotspot") {
                keyPopularityType =  KeyPopularity::hotSpotDistrib;
                distribIsRecognized = true;
            } else if(configValues["operator.key.popularity.distrib.type"] == "zipf") {
                keyPopularityType =  KeyPopularity::ZipfDistrib;
                distribIsRecognized = true;
            } else if(configValues["operator.key.popularity.distrib.type"] == "sequential") { // most of the time operators use this key
                keyPopularityType =  KeyPopularity::TemporalIncreasing;
                distribIsRecognized = true;
            } else if(configValues["operator.key.popularity.distrib.type"] == "uniform") {
                keyPopularityType =  KeyPopularity::UniformIntDistrib;
                distribIsRecognized = true;
            }

            uint64_t  keySpaceSize = 0;
            if (configValues.find("operator.key.popularity.distrib.params.keyspacesize") != configValues.end()) {
                try {
                    keySpaceSize = stoi(configValues["operator.key.popularity.distrib.params.keyspacesize"]);
                } catch (...) {
                    std::cout<<"Error:  operator&KeyPopularity: " << "Need  a number for  keyspacesize (operator.key.popularity.distrib.params.keyspacesize)" << std::endl;
                    return {};
                }
            } else {
                std::cout<<"Error: Can't make the new operator : " << " The  key space size (operator.key.popularity.distrib.params.keyspacesize) is not defined" << std::endl;
                return {};
            }

            // if the distribution type is not recognized, return
            if( distribIsRecognized != true) {
                std::cout<<"Error:  Can't make the new operator: " << "The KeyPopularity distribution type (operator.key.popularity.distrib.type) is not recognized" << std::endl;
                return {};
            }

            // make the distribution parameters
            auto  distributionParameters = std::make_shared<DistributionParameters>();


            switch (keyPopularityType) {
                case KeyPopularity::ConstantDistrib: // ConstInt is needed
                    if(configValues.find("operator.key.popularity.distrib.params.constant") != configValues.end()) {
                        try {
                            distributionParameters->constantString = configValues["operator.key.popularity.distrib.params.constant"];
                        } catch (...) {
                            std::cout<<"Error:  operator&KeyPopularity: " << "Need a constant string for key popularity distribution (operator.key.popularity.distrib.params.constant)" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  operator&KeyPopularity: " << "Need a constant string for key popularity distribution (operator.key.popularity.distrib.params.constant)" << std::endl;
                        return {};
                    }
                    break;
                case KeyPopularity::hotSpotDistrib: // hotspotfraction and hotoperationfraction are needed
                    if(configValues.find("operator.key.popularity.distrib.params.hotspotfraction") != configValues.end()) {
                        try {
                            distributionParameters->hotSpotFraction = stod(configValues["operator.key.popularity.distrib.params.hotspotfraction"]);
                        } catch (...) {
                            std::cout<<"Error:  operator&KeyPopularity: " << "KeyPopularity - HotSpotFraction (operator.key.popularity.distrib.params.hotspotfraction)  must be a real number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  operator&KeyPopularity: " << "KeyPopularity - hotspotfraction (operator.key.popularity.distrib.params.hotspotfraction) must be defined" << std::endl;
                        return {};
                    }
                    if(configValues.find("operator.key.popularity.distrib.params.hotoperationfraction") != configValues.end()) {
                        try {
                            distributionParameters->hotSpotFraction = stod(configValues["operator.key.popularity.distrib.params.hotoperationfraction"]);
                        } catch (...) {
                            std::cout<<"Error:  operator&KeyPopularity: "<< "KeyPopularity - hotoperationfraction (operator.key.popularity.distrib.params.hotoperationfraction)  must be a real number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  operator&KeyPopularity: "<< "KeyPopularity - hotoperationfraction (operator.key.popularity.distrib.params.hotoperationfraction) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case KeyPopularity::ZipfDistrib: // s is needed
                    if(configValues.find("operator.key.popularity.distrib.params.s") != configValues.end()) {
                        try {
                            distributionParameters->s = stod(configValues["operator.key.popularity.distrib.params.s"]);
                        } catch (...) {
                            std::cout<<"Error:  operator&KeyPopularity: " << "The s parameter (operator.key.popularity.distrib.params.s)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  operator&KeyPopularity: " << "The s parameter (operator.key.popularity.distrib.params.s) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case KeyPopularity::TemporalIncreasing: // no need for any parameter
                    break;

                case KeyPopularity::UniformIntDistrib: // need a and b
                    if(configValues.find("operator.key.popularity.distrib.params.a") != configValues.end()) {
                        try {
                            distributionParameters->a_real = stod(configValues["operator.key.popularity.distrib.params.a"]);
                        } catch (...) {
                            std::cout<<"Error:  operator&KeyPopularity: " << "KeyPopularity- The a parameter (operator.key.popularity.distrib.params.a)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  operator&KeyPopularity: " << "The a parameter (operator.key.popularity.distrib.params.a) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("operator.key.popularity.distrib.params.b") != configValues.end()) {
                        try {
                            distributionParameters->b_real = stod(configValues["operator.key.popularity.distrib.params.b"]);
                        } catch (...) {
                            std::cout<<"Error:  operator&KeyPopularity: "<< "The b parameter  (operator.key.popularity.distrib.params.b) must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error:  operator&KeyPopularity: " << "The b parameter (operator.key.popularity.distrib.params.b) must be defined" << std::endl;
                        return {};
                    }
                    break;


            }
            // if the code is here all the distrib type and the needed params are ok- make the distribution
            return  KeyPopularityBuilder::BuildKeyPopularity(keyPopularityType, distributionParameters, keySpaceSize);
        } else { // the user does not have any config for this distribution.
            std::cout<<"Error:  operator&KeyPopularity: " << "KeyPopularity distribution must be defined. Please add 'operator.key.popularity.distrib.type' " << std::endl;
            return {};
        }
    }



    /***
     *  Each operator uses a key to access the state store(KV store). Here a distribution for the key size is defined
     */
    std::optional<std::shared_ptr<KeySize>>  operatorKeySizeDistrib() {
        if (configValues.find("operator.key.size.distrib.type") != configValues.end()) {
            KeySize::KeySizeDistribs keySizeType;
            bool distribIsRecognized =false;
            // check if the user have provided  all the needed parameters for his/her needed distribution; if not, print error and exit
            if(configValues["operator.key.size.distrib.type"] == "exponential") {
                keySizeType =  KeySize::ExponentialDistrib;
                distribIsRecognized = true;
            } else if(configValues["operator.key.size.distrib.type"] == "constant") {
                keySizeType =  KeySize::ConstantDistrib;
                distribIsRecognized = true;
            } else if(configValues["operator.key.size.distrib.type"] == "uniform") {
                keySizeType =  KeySize::UniformRealDistrib;
                distribIsRecognized = true;
            } else if(configValues["operator.key.size.distrib.type"] == "normal") {
                keySizeType =  KeySize::NormalDistrib;
                distribIsRecognized = true;
            }

            // if the distribution type is not recognized, return
            if( distribIsRecognized != true) {
                std::cout<<"Error: operator&KeySizeDistrib:" << "The distribution type (operator.key.size.distrib.type) is not recognized" << std::endl;
                return {};
            }

            // make the distribution parameters
            auto  distributionParameters = std::make_shared<DistributionParameters>();


            switch (keySizeType) {
                case KeySize::ExponentialDistrib: // lambda is needed
                    if(configValues.find("operator.key.size.distrib.params.lambda") != configValues.end()) {
                        try {
                            distributionParameters->lambda = stod(configValues["operator.key.size.distrib.params.lambda"]);
                        } catch (...) {
                            std::cout<<"Error: operator&KeySizeDistrib: " << "Lambda (operator.key.size.distrib.params.lambda) must be a number!" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exit
                        std::cout<<"Error: operator&KeySizeDistrib:" << "parameter Lambda (operator.key.size.distrib.params.lambda) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case KeySize::ConstantDistrib: // ConstInt is needed
                    if(configValues.find("operator.key.size.distrib.params.constant") != configValues.end()) {
                        try {
                            distributionParameters->constantInt = stod(configValues["operator.key.size.distrib.params.constant"]);
                        } catch (...) {
                            std::cout<<"Error: operator&KeySizeDistrib: " << "The constant (operator.key.size.distrib.params.constant)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&KeySizeDistrib: " << "The constant (operator.key.size.distrib.params.constant) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case KeySize::NormalDistrib: // sd an mu is needed
                    if(configValues.find("operator.key.size.distrib.params.sd") != configValues.end()) {
                        try {
                            distributionParameters->sd = stod(configValues["operator.key.size.distrib.params.sd"]);
                        } catch (...) {
                            std::cout<<"Error: operator&KeySizeDistrib: " << "The sd parameter (operator.key.size.distrib.params.sd)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&KeySizeDistrib: " << "The sd parameter (operator.key.size.distrib.params.sd) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("operator.key.size.distrib.params.mean") != configValues.end()) {
                        try {
                            distributionParameters->mean = stod(configValues["operator.key.size.distrib.params.mean"]);
                        } catch (...) {
                            std::cout<<"Error: operator&KeySizeDistrib: " << "The mean parameter (operator.key.size.distrib.params.mean)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&KeySizeDistrib: " << "The mean parameter (operator.key.size.distrib.params.mean) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case KeySize::UniformRealDistrib: // a and b are needed
                    if(configValues.find("operator.key.size.distrib.params.a") != configValues.end()) {
                        try {
                            distributionParameters->a_real = stod(configValues["operator.key.size.distrib.params.a"]);
                        } catch (...) {
                            std::cout<<"Error: operator&KeySizeDistrib: " << "The a parameter (operator.key.size.distrib.params.a)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&KeySizeDistrib: " << "The a parameter (operator.key.size.distrib.params.a) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("operator.key.size.distrib.params.b") != configValues.end()) {
                        try {
                            distributionParameters->b_real = stod(configValues["operator.key.size.distrib.params.b"]);
                        } catch (...) {
                            std::cout<<"Error: operator&KeySizeDistrib: " << "The b parameter (operator.key.size.distrib.params.b)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&KeySizeDistrib: " << "The b parameter (operator.key.size.distrib.params.b) must be defined" << std::endl;
                        return {};
                    }

            }

            return  KeySizeBuilder::BuildKeySize(keySizeType, distributionParameters);

        } else {
            std::cout<<"Error: operator&KeySizeDistrib: " << " The key size distribution (operator.key.size.distrib.type) must be defined" << std::endl;
            return {};
        }
    }



    /**
     *  Each time the operator does a put (write) on the state store,  it needs a value. This value has a size. Here the value size
     *  distribution is defined
     * @return value size distribution
     */
    std::optional<std::shared_ptr<Value>>  operatorValueSizeDistrib() {
        if (configValues.find("operator.value.size.distrib.type") != configValues.end()) {
            // second lets find the distribution tyep
            Value::ValueDistribs ValueType;
            bool distribIsRecognized =false;
            // check if the user have provided  all the needed parameters for his/her needed distribution; if not, print error and exit
            if(configValues["operator.value.size.distrib.type"] == "exponential") {
                ValueType =  Value::ExponentialDistrib;
                distribIsRecognized = true;
            } else if(configValues["operator.value.size.distrib.type"] == "constant") {
                ValueType =  Value::ConstantDistrib;
                distribIsRecognized = true;
            } else if(configValues["operator.value.size.distrib.type"] == "uniform") {
                ValueType =  Value::UniformRealDistrib;
                distribIsRecognized = true;
            } else if(configValues["operator.value.size.distrib.type"] == "normal") {
                ValueType =  Value::NormalDistrib;
                distribIsRecognized = true;
            }

            // if the distribution type is not recognized, return
            if( distribIsRecognized != true) {
                std::cout<<"Error: operator&ValueSizeDistrib:" << "The distribution type (operator.value.size.distrib.type) is not recognized" << std::endl;
                return {};
            }

            // make the distribution parameters
            auto  distributionParameters = std::make_shared<DistributionParameters>();


            switch (ValueType) {
                case Value::ExponentialDistrib: // lambda is needed
                    if(configValues.find("operator.value.size.distrib.params.lambda") != configValues.end()) {
                        try {
                            distributionParameters->lambda = stod(configValues["operator.value.size.distrib.params.lambda"]);
                        } catch (...) {
                            std::cout<<"Error: operator&ValueSizeDistrib: " << "Lambda (perator.value.size.distrib.params.lambda) must be a number!" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&ValueSizeDistrib:" << "parameter Lambda (perator.value.size.distrib.params.lambda) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Value::ConstantDistrib: // ConstInt is needed
                    if(configValues.find("operator.value.size.distrib.params.constant") != configValues.end()) {
                        try {
                            distributionParameters->constantInt = stoi(configValues["operator.value.size.distrib.params.constant"]);
                        } catch (...) {
                            std::cout<<"Error: operator&ValueSizeDistrib: " << "The constant (operator.value.size.distrib.params.constant)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&ValueSizeDistrib: " << "The constant (operator.value.size.distrib.params.constant) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Value::NormalDistrib: // sd an mu is needed
                    if(configValues.find("operator.value.size.distrib.params.sd") != configValues.end()) {
                        try {
                            distributionParameters->sd = stod(configValues["operator.value.size.distrib.params.sd"]);
                        } catch (...) {
                            std::cout<<"Error: operator&ValueSizeDistrib: " << "The sd parameter (operator.value.size.distrib.params.sd)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&ValueSizeDistrib: " << "The sd parameter (operator.value.size.distrib.params.sd) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("operator.value.size.distrib.params.mean") != configValues.end()) {
                        try {
                            distributionParameters->mean = stod(configValues["operator.value.size.distrib.params.mean"]);
                        } catch (...) {
                            std::cout<<"Error: operator&ValueSizeDistrib: " << "The mean parameter  (operator.value.size.distrib.params.mean) must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&ValueSizeDistrib: " << "The mean parameter  (operator.value.size.distrib.params.mean) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case Value::UniformRealDistrib: // a and b are needed
                    if(configValues.find("operator.value.size.distrib.params.a") != configValues.end()) {
                        try {
                            distributionParameters->a_real = stod(configValues["operator.value.size.distrib.params.a"]);
                        } catch (...) {
                            std::cout<<"Error: operator&ValueSizeDistrib: " << "The a parameter (operator.value.size.distrib.params.a)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&ValueSizeDistrib: " << "The a parameter (operator.value.size.distrib.params.a) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("operator.value.size.distrib.params.b") != configValues.end()) {
                        try {
                            distributionParameters->b_real = stod(configValues["operator.value.size.distrib.params.b"]);
                        } catch (...) {
                            std::cout<<"Error: operator&ValueSizeDistrib: " << "The b parameter (operator.value.size.distrib.params.b)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&ValueSizeDistrib: " << "The b parameter (operator.value.size.distrib.params.b) must be defined" << std::endl;
                        return {};
                    }

            }

            // if the code is here all the distrib type and the needed params are ok- make the distribution]
           return ValueBuilder::BuildValue(ValueType, distributionParameters).value();

        } else { // the user does not have any config for this distribution.
            std::cout<<"Error: operator&ValueSizeDistrib: " << " The Value size distribution (operator.value.size.distrib.type) must be defined" << std::endl;
            return {};
        }
    }


    /***
     * The streaming system operator processes events one by one. Processing each request takes time. Service time distribution shows the service rate  of
     * streaming system -
     * @return
     */
    std::optional<std::shared_ptr<ServiceTime>>  operatorServiceTimeDistrib() {
        if (configValues.find("operator.service.time.distrib.type") != configValues.end()) {
            ServiceTime::ServiceTimeDistribs ServiceTimeType;
            bool distribIsRecognized =false;
            // check if the user have provided  all the needed parameters for his/her needed distribution; if not, print error and exit
            if(configValues["operator.service.time.distrib.type"] == "exponential") {
                ServiceTimeType =  ServiceTime::ExponentialDistrib;
                distribIsRecognized = true;
            } else if(configValues["operator.service.time.distrib.type"] == "constant") {
                ServiceTimeType =  ServiceTime::ConstantDistrib;
                distribIsRecognized = true;
            } else if(configValues["operator.service.time.distrib.type"] == "uniform") {
                ServiceTimeType =  ServiceTime::UniformRealDistrib;
                distribIsRecognized = true;
            } else if(configValues["operator.service.time.distrib.type"] == "normal") {
                ServiceTimeType =  ServiceTime::NormalDistrib;
                distribIsRecognized = true;
            }

            // if the distribution type is not recognized, return
            if( distribIsRecognized != true) {
                std::cout<<"Error: operator&ServiceTimeDistrib:" << "The distribution type (operator.service.time.distrib.type) is not recognized" << std::endl;
                return {};
            }

            // make the distribution parameters
            auto  distributionParameters = std::make_shared<DistributionParameters>();


            switch (ServiceTimeType) {
                case ServiceTime::ExponentialDistrib: // lambda is needed
                    if(configValues.find("operator.service.time.distrib.params.lambda") != configValues.end()) {
                        try {
                            distributionParameters->lambda = stod(configValues["operator.service.time.distrib.params.lambda"]);
                        } catch (...) {
                            std::cout<<"Error: operator&ServiceTimeDistrib: " << "Lambda (operator.service.time.distrib.params.lambda) must be a number!" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&ServiceTimeDistrib:" << "parameter Lambda (operator.service.time.distrib.params.lambda) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case ServiceTime::ConstantDistrib: // ConstInt is needed
                    if(configValues.find("operator.service.time.distrib.params.constant") != configValues.end()) {
                        try {
                            distributionParameters->constantReal = stod(configValues["operator.service.time.distrib.params.constant"]);
                        } catch (...) {
                            std::cout<<"Error: operator&ServiceTimeDistrib: " << "The constant (operator.service.time.distrib.params.constant)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&ServiceTimeDistrib: " << "The constant (operator.service.time.distrib.params.constant) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case ServiceTime::NormalDistrib: // sd an mu is needed
                    if(configValues.find("operator.service.time.distrib.params.sd") != configValues.end()) {
                        try {
                            distributionParameters->sd = stod(configValues["operator.service.time.distrib.params.sd"]);
                        } catch (...) {
                            std::cout<<"Error: operator&ServiceTimeDistrib: " << "The sd parameter (operator.service.time.distrib.params.sd)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&ServiceTimeDistrib: " << "The sd parameter (operator.service.time.distrib.params.sd) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("operator.service.time.distrib.params.mean") != configValues.end()) {
                        try {
                            distributionParameters->mean = stod(configValues["operator.service.time.distrib.params.mean"]);
                        } catch (...) {
                            std::cout<<"Error: operator&ServiceTimeDistrib: " << "The mean parameter (operator.service.time.distrib.params.mean)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&ServiceTimeDistrib: " << "The mean parameter (operator.service.time.distrib.params.mean) must be defined" << std::endl;
                        return {};
                    }
                    break;
                case ServiceTime::UniformRealDistrib: // a and b are needed
                    if(configValues.find("operator.service.time.distrib.params.a") != configValues.end()) {
                        try {
                            distributionParameters->a_real = stod(configValues["operator.service.time.distrib.params.a"]);
                        } catch (...) {
                            std::cout<<"Error: operator&ServiceTimeDistrib: " << "The a parameter (operator.service.time.distrib.params.a)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&ServiceTimeDistrib: " << "The a parameter (operator.service.time.distrib.params.a) must be defined" << std::endl;
                        return {};
                    }

                    if(configValues.find("operator.service.time.distrib.params.b") != configValues.end()) {
                        try {
                            distributionParameters->b_real = stod(configValues["operator.service.time.distrib.params.b"]);
                        } catch (...) {
                            std::cout<<"Error: operator&ServiceTimeDistrib: " << "The b parameter (operator.service.time.distrib.params.b)  must be a number" << std::endl;
                            return {};
                        }
                    } else { // the needed  parameter does not exist
                        std::cout<<"Error: operator&ServiceTimeDistrib: " << "The b parameter (operator.service.time.distrib.params.b) must be defined" << std::endl;
                        return {};
                    }

            }
            // if the code is here all the distrib type and the needed params are ok- make the distribution
            return ServiceTimeBuilder::BuildServiceTime(ServiceTimeType, distributionParameters).value();

        } else { // the user does not have any config for this distribution.
            //std::cout<<"Error: operator&ServiceTimeDistrib: " << " The service  time  distribution (operator.service.time.distrib.type) be defined" << std::endl;
            return {};
        }
    }


/////////////////////////////////////////////////////////////
// build the operator
//////////////////////////////////////////////////////////////

std::optional<std::shared_ptr<Operator>> makeOperator ( ) {
    auto operatorParams = std::make_shared<OperatorParameters>();


    Operator::operatorName operatorType_ = Operator::none;
    // if the operator is a window operator, as apposed to continuous operators
    bool operatorIsWindow = false;
    //if the operator is  of type of sliding
    bool operatorIsSliding = false;
    // if this a two stream operator (e.g., join)
    bool twoStreamOperator = false;

    bool isIntervalJoin = false;


    //if the operator is yscb or replayer
    bool isReplayer = false;

    if (configValues.find("operator.type") != configValues.end()) {

        std::string oprName = configValues["operator.type"];
        if (oprName == "tumbling.all.incremental") {
            operatorType_ = Operator::OperatorWindowTumblingAllIncremental;
            operatorIsWindow = true;
        } else if (oprName == "tumbling.all.holistic") {
            operatorType_ = Operator::OperatorWindowTumblingAllHolistic;
            operatorIsWindow = true;
        } else if (oprName == "tumbling.keyed.incremental") {
            operatorType_ = Operator::OperatorWindowTumblingKeyedIncremental;
            operatorIsWindow = true;
        } else if (oprName == "tumbling.keyed.holistic") {
            operatorType_ = Operator::OperatorWindowTumblingKeyedHolistic;
            operatorIsWindow = true;
        } else if (oprName == "ycsb.replayer") {
            std::cout << "yscb replayer" << std::endl;
            isReplayer = true;
            operatorType_ = Operator::OperatorYCSBTrace;
        } else if (oprName == "sliding.all.incremental") {
            operatorType_ = Operator::OperatorWindowSlidingAllIncremental;
            operatorIsWindow = true;
            operatorIsSliding = true;
        } else if (oprName == "sliding.all.holistic") {
            operatorType_ = Operator::OperatorWindowSlidingAllHolistic;
            operatorIsWindow = true;
            operatorIsSliding = true;
        } else if (oprName == "sliding.keyed.incremental") {
            operatorType_ = Operator::OperatorWindowSlidingKeyedIncremental;
            operatorIsWindow = true;
            operatorIsSliding = true;
        } else if (oprName == "sliding.keyed.holistic") {
            operatorType_ = Operator::OperatorWindowSlidingKeyedHolistic;
            operatorIsWindow = true;
            operatorIsSliding = true;
        } else if (oprName == "join.tumbling") {
            operatorType_ = Operator::OperatorJoinTumbling;
            twoStreamOperator = true;
            operatorIsWindow = true;
        } else if (oprName == "join.sliding") {
            operatorType_ = Operator::OperatorJoinSliding;
            twoStreamOperator = true;
            operatorIsWindow = true;
            operatorIsSliding = true;
        } else if (oprName == "continuous.aggregation") {
            operatorType_ = Operator::OperatorContinuousAggregation;
        } else if (oprName == "flink.replayer") {
            isReplayer = true;
            operatorType_ = Operator::OperatorFlinkTrace;
        } else if (oprName == "join.interval") {
            isIntervalJoin = true;
            operatorType_ = Operator::OperatorJoinInterval;
            twoStreamOperator = true;
        } else if (oprName == "continuous.join") { // fixme
            twoStreamOperator = true;
            operatorType_ = Operator::OperatorContinuousJoin;
            twoStreamOperator = true;
        } else { // set the default
            operatorType_ = Operator::none;
        }

        // no operator is defined
        if (operatorType_ == Operator::none) {
            std::cout << "Error: Cant build the  operator : " << " Operator type (operator.type) is not known"
                      << std::endl;
           throw std::exception();
        } else {
            operatorParams->operatorType = operatorType_;
        }

        // get the expected number of operations
        if (configValues.find("expected.num.operations") != configValues.end()) {
            try {
                operatorParams->expectedNumOperations = stoi(configValues["expected.num.operations"]);
                expectedNumOpr = operatorParams->expectedNumOperations;
            } catch (...) {
                std::cout << "Error: Cant build the  operator : " << "Need  a number for  expected.num.operations"
                          << std::endl;
                throw std::exception();
            }
        } else { // set the default
            std::cout << "Error: Cant build the  operator : " << " The number of expected operations (expected.num.operations) must be defined "
                      << std::endl;
            throw std::exception();
        }


        // get the window length
        if (configValues.find("window.length") != configValues.end()) {
            try {
                operatorParams->windowLength = stoi(configValues["window.length"]);
            } catch (...) {
                std::cout << "Error:  operator building: " << "Need  a number for  window length (window.length) " << std::endl;
                return {};
            }
        } else { // set the default
            if (operatorIsWindow) { // if the operator is of type window- window type is needed
                std::cout << "Error:  operator building: " << "window length (window.length) must be defined " << std::endl;
               throw std::exception();
            }
        }

        // get  allowedLateness
        if (configValues.find("allowed.lateness") != configValues.end()) {
            if (configValues["allowed.lateness"] == "true") {
                operatorParams->allowedLateness = true;
            } else if (configValues["allowed.lateness"] == "false") {
                operatorParams->allowedLateness = false;
            } else {
                std::cout << "Error:  operator building: " << "Need   true or false for allowed lateness (allowed.lateness)"
                          << std::endl;
                throw std::exception();
            }
        } else { // set the default
            operatorParams->allowedLateness = false;
        }

        // get the lateness length
        if (configValues.find("lateness.length") != configValues.end()) {
            try {
                operatorParams->latenessLength = stoi(configValues["lateness.length"]);
            } catch (...) {
                std::cout << "Error:  operator building: " << "Need  a number for  lateness.length" << std::endl;
                throw std::exception();
            }
        } else { //
            if (operatorParams->allowedLateness) { // if lateness allowed, lateness length is needed
                std::cout << "Error:  operator building: " << "lateness  length (lateness.length) must be defined " << std::endl;
                throw std::exception();
            }

        }


        // get the sliding length in terms of unit of time
        if (configValues.find("sliding.length") != configValues.end()) {
            try {
                operatorParams->slidingLength = stoi(configValues["sliding.length"]);
            } catch (...) {
                std::cout << "Error:  operator building: " << "Need  a number for  sliding.length" << std::endl;
                return {};
            }
        } else {
            if (operatorIsSliding) {
                std::cout << "Error:  operator building: " << "sliding  length (sliding.length) must be defined " << std::endl;
                throw std::exception();
            }
        }

        auto firstEventGenerator = buildFirstEventGenerator();
        if( firstEventGenerator.has_value()) {
            operatorParams->eventGenerator = firstEventGenerator.value();
        } else {
            std::cout << "cant make the operator " << " the event source  is not defined"  << std::endl;
            throw std::exception();
        }


        // if the second stream of events (i.e., event stream is needed)
        if (twoStreamOperator) {
            auto secondEventGenerator = buildSecondEventGenerator();
            if(secondEventGenerator.has_value()) {
                operatorParams->eventGenerator2 = secondEventGenerator.value();
            } else {
                std::cout << " Error: cant make the operator " << " the  second event source  is not defined"  << std::endl;
                throw std::exception();
            }
        }

        // make the service time distribution
        auto ServiceTimeDistrib = operatorServiceTimeDistrib();
        if(ServiceTimeDistrib.has_value()) {
            operatorParams->serviceTimeDistribution = ServiceTimeDistrib.value();
        } else {
            // if the user does not define any service time distribution, we define a
            // constant service  time distribution equal to zero
            auto  distributionParameters = std::make_shared<DistributionParameters>();
            distributionParameters->constantReal = 0;
            operatorParams->serviceTimeDistribution = ServiceTimeBuilder::BuildServiceTime(ServiceTime::ConstantDistrib, distributionParameters).value();
        }

        // make the value size distribution
        auto valueSizeDistrib = operatorValueSizeDistrib();
        if(valueSizeDistrib.has_value()) {
            operatorParams->valuesDistribution = valueSizeDistrib.value();
        } else {
            std::cout << "Error: cant make the operator " << " the  value size  distribution is not defined"  << std::endl;
            throw std::exception();
        }

        auto keyPopularityDistribution = operatorKeyPopularityDistrib();
        if(!keyPopularityDistribution.has_value()) {
            std::cout << "Error: cant make the operator " << " the  key popularity distribution for the operator   is not defined"  << std::endl;
            throw std::exception();
        }

        auto keySizeDistrib = operatorKeySizeDistrib();
        if(!keySizeDistrib.has_value()) {
            std::cout << "Error: cant make the operator " << " the  key size distribution for the operator   is not defined"  << std::endl;
            throw std::exception();
        }

        operatorParams->keysDistribution = KeyBuilder::BuildKey(keyPopularityDistribution.value(), keySizeDistrib.value());

        if(isReplayer) {
            if (configValues.find("trace.file.path") != configValues.end()) {
                    operatorParams->traceFilePath =configValues["trace.file.path"];
            } else {
                    std::cout << "Error:  trace.file.path: " << "trace file path (trace.file.path) for replayer is not defined " << std::endl;
            }
        }
        if(isIntervalJoin) {
            if (configValues.find("upper.bound") != configValues.end()) {
                try {
                    operatorParams->upperBound = stoi(configValues["upper.bound"]);
                } catch (...) {
                    std::cout << "Error:   interval join: " << "Need  an integer  for  upper.bound" << std::endl;
                    return  {};
                }
            } else {
                std::cout << "Error:  interval join " << "upper.bound is not defined " << std::endl;
                throw std::exception();
            }

            if (configValues.find("lower.bound") != configValues.end()) {
                try {
                    operatorParams->lowerBound = stoi(configValues["lower.bound"]);
                } catch (...)  {
                    std::cout << "Error:   interval join: " << "Need  an integer  for  lower.bound" << std::endl;
                    return  {};
                }
            } else {
                std::cout << "Error:  interval join " << "lower.bound is not defined " << std::endl;
                throw std::exception();
            }
        }

        // build the operator
        return  OperatorBuilder::BuildOperator(operatorParams);
    } else {
        std::cout << "Error: Cant build the  operator : " << " operator type (operator.type) is not known"
                  << std::endl;
        throw std::exception();
    }

}

std::optional<std::shared_ptr<KVWrapper>> makeWrapper() {
    if (configValues.find("wrapper.type") != configValues.end()) {
        std::string path = "";
        if (configValues.find("wrapper.path") != configValues.end()) {
            path = configValues["wrapper.path"];
        } else{
            std::cout << "Error: Cant build the  wrapper : " << " a path (wrapper.path) must be defined"
                      << std::endl;
            throw std::exception();
        }

        std::string wrapperType = configValues["wrapper.type"];
        isWrapperFile = false;
        if (wrapperType == "file") {
            isWrapperFile = true;
            return  WrapperBuilder::BuildWrapper(KVWrapper::File, path);
        } else if (wrapperType == "rocksdb") {
            return  WrapperBuilder::BuildWrapper(KVWrapper::RocksDB, path);
        } else if (wrapperType == "berkeley") {
            return  WrapperBuilder::BuildWrapper(KVWrapper::BerkeleyDB, path);
        } else if (wrapperType == "faster") {
            return  WrapperBuilder::BuildWrapper(KVWrapper::Faster, path);
        } else if (wrapperType == "lethe") {
            return  WrapperBuilder::BuildWrapper(KVWrapper::Lethe, path);
        } else {
            std::cout << "Error: Cant build the  wrapper : " << " wrapper(wrapper.type) is not known"
                      << std::endl;
            throw std::exception();
        }

    } else {
        std::cout << "Error: Cant build the  wrapper : " << " wrapper type (wrapper.type) must be defined (options: file,rocksdb, faster, berkeley, lethe)"
                  << std::endl;
        throw std::exception();
    }
}

bool dumpOnFile() {
    return  isWrapperFile;
    }
uint64_t  expectedNumOperation() {
    return expectedNumOpr;
}


private:
    // this dictionary holds all configuration parameters
    std::unordered_map<std::string, std::string>  configValues;
    bool isWrapperFile;
    uint64_t  expectedNumOpr = LONG_MAX;

};


