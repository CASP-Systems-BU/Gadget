//
// Created by Showan Asyabi on 1/19/21.
//
#include <iostream>
#include <memory>
#include <fstream>

#include "gadget/distributions/arrival.h"
#include "gadget/distributions/key.h"
#include "gadget/distributions/value.h"
#include "gadget/distributions/windowLength.h"
#include "gadget/kvwrappers/kvwrapper.h"
#include "gadget/kvwrappers/wrapperBuilder.h"
#include "gadget/performanceMetrics.h"
#include "gadget/distributions/distributionParamters.h"
#include "include/gadget/operator/operators/operatorParameters.h"
#include "include/gadget/operator/operators/operatorBuilder.h"
#include "gadget/event/eventGeneratorBuilder.h"
#include "tests.h"
#include "config.h"





int drive_machine(const std::string& pathToConfigFile) {

      std::unique_ptr<Config> config;
    try {
         config = std::make_unique<Config>(pathToConfigFile);
    } catch (std::exception&) {
        std::cout<< "Error: cannot find the config file. Execute Gadget as follows:'gadget config.txt'"<<std::endl;
        return EXIT_FAILURE;
    }

    std::optional<std::shared_ptr<Operator>> newOperator;
    try {
        newOperator = config->makeOperator();
        if(!newOperator.has_value()) {
            std::cout << "Error:  The operator is not known" << std::endl;
            return 0;
        }
    } catch (std::exception&) {
        std::cout<< "Error: The operator cannot be made!"<<std::endl;
        return EXIT_FAILURE;
    }


  // here we generate operations
    uint64_t  numberProducedOperation = 0;
    uint64_t  numberProducedMerges = 0;
    uint64_t  numberProducedDeletes = 0;
    uint64_t  numberProducedGets = 0;
    uint64_t  numberProducedPuts = 0;
    uint64_t  numberProducedSeeks = 0;
    uint64_t  numberProducedNexts = 0;

    bool dumpOnFile = true;
    // to check if the user expected number of operations are generated or not
    bool enoughOprGenerated = false;

    std::optional<std::shared_ptr<KVWrapper>> wrapper;
    try {
            wrapper = config->makeWrapper();
            dumpOnFile = config->dumpOnFile();
            if(!wrapper.has_value()) {
                std::cout << " Error:  The wrapper is not known" << std::endl;
                return 0;
            }
    }  catch (std::exception&) {
        std::cout<< "Error: The wrapper cannot be made!"<<std::endl;
        return EXIT_FAILURE;
    }

    // build a wrapper - wrapper can be a db or a file
    // if the wrapper is File the generated operations are dumped to a file
    // if the wrapper is a DB (e.g. rocksDB). the DB is stressed with the generated operations
    if(!wrapper.value()) {
        std::cout << "Error: cannot make the db wrapper!" << std::endl;
        exit(0);
    }
    auto performance =  std::make_shared<PerformanceMetrics>(wrapper.value());

    // operations that are generated for the next  time unit
    std::vector<std::shared_ptr<Operation>> operationsBatch;

    std::cout << "Generating operations ..." << std::endl;
    while (true) {
        operationsBatch.clear();
        bool operationsLeft = newOperator.value()->runOperator(operationsBatch);
        //numberProducedOperation += operationsBatch.size();
       // std::cout << "num of generated operations" << numberProducedOperation << std::endl;
        for (auto opr : operationsBatch ) {
            numberProducedOperation ++;
            wrapper.value()->operationsList.push_back(opr->oprType);
            wrapper.value()->keysList.push_back(opr->key);
            //std::cout << "key" <<  wrapper.value()->keysList.back() << std::endl;
            wrapper.value()->valuesList.push_back(opr->value);
            //std::cout << "value" <<  wrapper.value()->valuesList.back() << std::endl;
           switch (static_cast<int>(wrapper.value()->operationsList.back())) {
                case 0:
                    //std::cout << "get" << std::endl;
                    numberProducedGets++;
                    break;
                case 1:
                    //std::cout << "put" << std::endl;
                    numberProducedPuts++;
                    break;
                case 2:
                    //std::cout << "merge" << std::endl;
                    numberProducedMerges++;
                    wrapper.value()->mergeExist = true;
                   break;
                case 3:
                    //std::cout << "delete" << std::endl;
                    numberProducedDeletes++;
                    break;
                case 4:
                    //std::cout << "scan" << std::endl;
                    numberProducedSeeks++;
                    break;
                case 5:
                    //std::cout << "next" << std::endl;
                    numberProducedNexts++;
                    break;
               default:
                   std::cout<< "Operation is not known" << std::endl;
            }

           if(numberProducedOperation >= config->expectedNumOperation()) {
               enoughOprGenerated = true;
               break;
           }
        }
        if (operationsLeft == false || enoughOprGenerated) {
            break;
        }
    }

    if (!wrapper.value()->connect()) {
        std::cout << "Cant connect to the wrapper ..." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Number of generated operations: "<<numberProducedOperation   << std::endl;
    std::cout << "Number of generated merges: "<<numberProducedMerges   << std::endl;
    std::cout << "Number of generated puts: "<<numberProducedPuts  << std::endl;
    std::cout << "Number of generated gets: "<<numberProducedGets   << std::endl;
    std::cout << "Number of generated deletes: "<<numberProducedDeletes   << std::endl;
    std::cout << "Number of generated seeks: "<<numberProducedSeeks   << std::endl;
    std::cout << "Number of generated  nexts: "<<numberProducedNexts  << std::endl;

    std::cout << "Operations are generated  successfully :)" <<std::endl;

    /***
     * here we execute  the  generated commands on KV store or dump them on the file
     */

    /***
     * here we show  user the performance metrics
     */
    std::cout<< "******************** Results **********************" << std::endl;
    std::cout<< "Number of operations: " << numberProducedOperation << std::endl;
    std::cout<< "Key space size: " <<  wrapper.value()->keysList.back() << std::endl;


    if(dumpOnFile) {
        wrapper.value()->dumpOperationsOnFile();
        wrapper.value()->disconnect();
    } else {
        // quantify the throughput of the db
        double  throughput = wrapper.value()->runBatchThroughput();
        wrapper.value()->disconnect();

        // quantify the latency of the individual requests
        wrapper.value()->connect();
        wrapper.value()->runBatchLatency();
        wrapper.value()->disconnect();
        std::cout<<"Throughput:" <<throughput * 1000000<<" opr/sec" <<std::endl;
        std::cout << "P99.99: " << performance->ExpP99_99() << " us" << std::endl;
        std::cout << "P99.9: " << performance->ExpP99_9() << " us" << std::endl;
        std::cout << "P99: " << performance->ExpP99() << " us" << std::endl;
        std::cout << "P50: " << performance->ExpP50() << " us" << std::endl;
        std::cout << "Max Response Time: " << performance->ExpMax() << " us" << std::endl;
        std::cout << "Min Response Time: " << performance->ExpMin() << " us" << std::endl;
        std::cout << "AVG Response Time: " << performance->ExpAvg() << " us" << std::endl;

    }





#if 0
/***
 *  here we make  the event generators
 */
    /***
     * This event generator generates 10 requests per second according to poisson process
     */
    auto eventDistributionParams = std::make_shared<DistributionParameters>();
    eventDistributionParams -> lambda = 200;
    auto eventTimeDistribution = ArrivalBuilder::BuildArrival(Arrival::ExponentialDistrib, eventDistributionParams);


    /***
     * Here we define the key popularity of user keys- For example, here we say user keys is
     * uniformly distributed from 1 to 100
     */
    auto ekeyPopularityDistributionParams = std::make_shared<DistributionParameters>();
    ekeyPopularityDistributionParams->s = 1; // this is in a zipf distribution
    //ekeyPopularityDistributionParams->hotSpotFraction = 0.03;
    //ekeyPopularityDistributionParams->hotOperationFraction = 0.85;
    ekeyPopularityDistributionParams->pathToFile = "taskkeylist.csv";
    auto ekeyDistribution = KeyPopularityBuilder::BuildKeyPopularity(KeyPopularity::FileKeys ,ekeyPopularityDistributionParams, 9000);
    //auto ekeyDistribution = std::make_shared<ECDF>("ecdf-keyFreq.csv");

    /***
     * Here we define the the distribution of arrivals of events to the system
     *
     */
    auto arrivalTimeDistributionParams = std::make_shared<DistributionParameters>();
    arrivalTimeDistributionParams -> lambda =10;
    auto ArrivalDistribution = ArrivalBuilder::BuildArrival(Arrival::ExponentialDistrib, arrivalTimeDistributionParams);



    auto eventsPerWindowDistrib =  std::make_shared<ECDFWindowLength>("ecdf-windowLength.csv");
    arrivalTimeDistributionParams->constantInt = 500;
    //auto eventsPerWindowDistrib =  WindowLengthBuilder::BuildWindowLength(WindowLength::ConstantDistrib,arrivalTimeDistributionParams);
    /***
     * here we build two event generators
     */
    //auto eventGen = std::make_shared<EventGenerator>(eventTimeDistribution.value(), ArrivalDistribution.value(), ekeyDistribution.value());
    //auto eventGen2 = std::make_shared<EventGenerator>(eventTimeDistribution.value(), ArrivalDistribution.value(), ekeyDistribution.value());
    auto  eventGenParams  = std::make_shared<EventGeneratorParameters>();

    eventGenParams->keyPopularity = ekeyDistribution.value();

    eventGenParams->eventTimesDistribution = eventTimeDistribution.value();
    eventGenParams->eventRateDistribution = eventsPerWindowDistrib;
    eventGenParams->eventRateChangeFrequency = 5; // window length

    eventGenParams->arrivalTimesDistribution = ArrivalDistribution.value();
    auto arrivalRateChangeFrequencyDistParams = std::make_shared<DistributionParameters>();
    arrivalRateChangeFrequencyDistParams->constantInt = 50; // 50 events per window/ 10 per unit(ie., pane)
    eventGenParams->arrivalRateDistribution = WindowLengthBuilder::BuildWindowLength(WindowLength::ConstantDistrib, arrivalRateChangeFrequencyDistParams).value();
    eventGenParams->arrivalRateChangeFrequency =5;
    //auto eventGen = std::make_shared<EventGenerator>(eventTimeDistribution.value(), ArrivalDistribution.value(), ekeyDistribution, windowLenghtDistribution);
    //auto eventGen2 = std::make_shared<EventGenerator>(eventTimeDistribution.value(), ArrivalDistribution.value(), ekeyDistribution , windowLenghtDistribution);

    //auto eventGen =  EventGeneratorBuilder::BuildEventGenerator(EventGenerator::Main, eventGenParams);
    auto eventGen2 =  EventGeneratorBuilder::BuildEventGenerator(EventGenerator::Main, eventGenParams);
#endif



#if 0

    // events that generated by borg input file
    auto eventGeneratorParams = std::make_shared<EventGeneratorParameters>();
    eventGeneratorParams->keyIndex = 0;
    eventGeneratorParams->eventTimeIndex = 2;
    eventGeneratorParams->eventFilePath = "tasks-input.txt";
    eventGeneratorParams->waterMarkFrequency = 1;
    //eventGeneratorParams->eventFilePath = "mytasks2.txt";
    //eventGeneratorParams->filetype = EventGeneratorParameters::BorgTaskFile;
    auto eventGen  =  EventGeneratorBuilder::BuildEventGenerator(EventGenerator::TraceFile, eventGeneratorParams);

    //eventGeneratorParams->eventFilePath = "jobs-input-filtered.txt";
    eventGeneratorParams->eventFilePath = "jobs.txt";
    eventGeneratorParams->keyIndex = 0;
    eventGeneratorParams->eventTimeIndex = 1;
    eventGeneratorParams->waterMarkFrequency = 1;
    //eventGeneratorParams->eventFilePath = "myjobs2.txt";
    //eventGeneratorParams->filetype = EventGeneratorParameters::BorgJobFile;
    auto eventGen2  =  EventGeneratorBuilder::BuildEventGenerator(EventGenerator::TraceFile, eventGeneratorParams);


/***
 * Here we make all the needed distribution
 */
   /**
    * We first make the service time distribution
    */
   auto serviceTimeDistributionParams = std::make_shared<DistributionParameters>();
   serviceTimeDistributionParams->lambda = 10;
   auto  serviceTimeDistribution = ServiceTimeBuilder::BuildServiceTime(ServiceTime::ExponentialDistrib, serviceTimeDistributionParams);


     /**
       Here we make the distribution of the size of the values- for example here the size of
      * values is constant 10
     */
   auto valueDistributionParams = std::make_shared<DistributionParameters>();
   valueDistributionParams->constantInt = 10;
   auto valueDistribution = ValueBuilder::BuildValue(Value::ConstantDistrib, valueDistributionParams);

    /**
       Here we make the distribution of the window keys of the values- for example here distribution is TemporalIncreasing
       meaning that for each new window there is new key
     */

    /**
       Here we  also make the distribution of the  key sizes - for example here distribution  of key sizes is
       constant and it is equal 10 bytes
     */
   auto keyPopularityDistributionParams = std::make_shared<DistributionParameters>();
   auto keySizeDistributionParams = std::make_shared<DistributionParameters>();
   keySizeDistributionParams -> constantInt = 10;
   auto keyPopDist =  KeyPopularityBuilder::BuildKeyPopularity(KeyPopularity::TemporalIncreasing,keyPopularityDistributionParams , 10000000);
   auto keySizeDist =  KeySizeBuilder::BuildKeySize(KeySize::ConstantDistrib, keySizeDistributionParams);
   auto keyDistribution = KeyBuilder::BuildKey(keyPopDist.value(),keySizeDist.value());

   /**
    * if the any of the distribution are not valid, do not proceed
    */
   if(!serviceTimeDistribution.has_value() || !valueDistribution.has_value() ) {
        std::cout<< "One of the distribution is not valid";
       return 0;
   }

   /***
    * here we are building the operator
    */

   auto operatorParams = std::make_shared<OperatorParameters>();
   operatorParams->expectedNumOperations = 100000;
   operatorParams->windowLength = 5;
   operatorParams->allowedLateness = false;
   operatorParams->latenessLength = 0;
   operatorParams->slidingLength = 1;
   operatorParams->eventGenerator= eventGen.value();
   operatorParams->eventGenerator2 = eventGen2.value();
   operatorParams->serviceTimeDistribution = serviceTimeDistribution.value();
   operatorParams->keysDistribution = keyDistribution;
   operatorParams->valuesDistribution = valueDistribution.value();
   operatorParams->stateMachineType = StateMachines::holistic;
   operatorParams->keyedOperator_ = true;
   /**
    * here we choose the operator type
    */
    operatorParams->operatorType = Operator::OperatorWindowTumblingAllHolistic;
    auto newOperator = OperatorBuilder::BuildOperator(operatorParams);


   if(!newOperator.has_value()) {
       std::cout<< "the operator is not valid";
       return 0;
   }

    std::vector<std::shared_ptr<Operation>> vec;
    uint64_t  num_generateOperation = 0;


    /***
    * here we call the operator for the next Key&Value&operation
    */
    uint64_t  numberProducedOperation = 0;
    uint64_t  numberProducedMerges = 0;
    uint64_t  numberProducedDeletes = 0;
    uint64_t  numberProducedGets = 0;
    uint64_t  numberProducedPuts = 0;
    uint64_t  numberProducedSeeks = 0;
    uint64_t  numberProducedNexts = 0;
   auto  wrapper= WrapperBuilder::BuildWrapper(KVWrapper::FileSystem);
    //auto  wrapper= WrapperBuilder::BuildWrapper(KVWrapper::RocksDB);
   if(!wrapper.value()) {
       std::cout << "Error: cannot make the db wrapper!" << std::endl;
       exit(0);
   }
   auto performance =  std::make_shared<PerformanceMetrics>(wrapper.value());
   wrapper.value()->connect();

   while (true) {
       vec.clear();
       bool ret = newOperator.value()->runOperator(vec);

       numberProducedOperation += vec.size();

       for (auto opr : vec ) {
           wrapper.value()->operationsList.push_back(opr->oprType);
           switch (static_cast<int>(wrapper.value()->operationsList.back())) {
               case 0:
                   //std::cout << "get" << std::endl;
                   numberProducedGets++;
                   break;
               case 1:
                   //std::cout << "put" << std::endl;
                   numberProducedPuts++;
                   break;
               case 2:
                   //std::cout << "merge" << std::endl;
                   numberProducedMerges++;
                   break;
               case 3:
                   //std::cout << "delete" << std::endl;
                   numberProducedDeletes++;
                   break;
               case 4:
                   //std::cout << "scan" << std::endl;
                   numberProducedSeeks++;
                   break;
               case 5:
                   //std::cout << "next" << std::endl;
                   numberProducedNexts++;
                   break;
           }


           wrapper.value()->keysList.push_back(opr->key);
           //std::cout << "key: " << opr->key<< std::endl;
           wrapper.value()->valuesList.push_back(opr->value);
           //std::cout << "value: " << opr->value<< std::endl;
           //wrapper.value()->sleepTimes.push_back();
           //std::cout << "number of done merges: " << numberProducedMerges << std::endl;
           //std::cout << "number of done operations: " << numberProducedOperation << std::endl;
           /*
           if(numberProducedDeletes >  15) {
               break;
           }
            */
       }

       if (ret == 0) {
           break;
       }
   }

    std::cout << "number of done operations: "<<numberProducedOperation   << std::endl;
    std::cout << "number of done merges: "<<numberProducedMerges   << std::endl;
    std::cout << "number of done puts: "<<numberProducedPuts  << std::endl;
    std::cout << "number of done gets: "<<numberProducedGets   << std::endl;
    std::cout << "number of done delets: "<<numberProducedDeletes   << std::endl;
    std::cout << "number of done  seeks: "<<numberProducedSeeks   << std::endl;
    std::cout << "number of done  nexts: "<<numberProducedNexts  << std::endl;

    std::cout << "Finished successfully :D" <<std::endl;

   /***
    * here we execute  the  generated commands on KV store
    */
    //double throughput = wrapper.value()->runBatchThroughput();
    wrapper.value()->dumpOperationsOnFile();
    wrapper.value()->disconnect();



    //wrapper.value()->connect();
    //wrapper.value()->runBatchLatency();

    /***
     * here we show the user the performance metrics
     */
     /*
    std::cout<< "******************** Results **********************" << std::endl;
    std::cout<< "Number of operations: " << numberProducedOperation << std::endl;
    std::cout<<"Throughput:" <<throughput * 1000000<<" opr/Sec" <<std::endl;
    std::cout<<"P99.99: "<< performance->ExpP99_99()<< " us" << std::endl;
    std::cout<<"P99.9: "<< performance->ExpP99_9()<< " us" <<std::endl;
    std::cout<<"P99: "<< performance->ExpP99()<<" us" << std::endl;
    std::cout<<"Max Response Time: "<< performance->ExpMax()<< " us" << std::endl;
    std::cout<<"Min Response Time: "<< performance->ExpMin()<< " us" << std::endl;
    std::cout<<"AVG Response Time: "<< performance->ExpAvg()<< " us" << std::endl;
    wrapper.value()->disconnect();
     */
    return 0;


#if 0

   /***
    * here we call the operator for the next Key&Value&operation
    */
    uint64_t  numberProducedOperation = 0;
    uint64_t  numberProducedMerges = 0;
    uint64_t  numberProducedDeletes = 0;
    uint64_t  numberProducedGets = 0;
    uint64_t  numberProducedPuts = 0;
    uint64_t  numberProducedSeeks = 0;
    uint64_t  numberProducedNexts = 0;
   //auto  wrapper= WrapperBuilder::BuildWrapper(KVWrapper::FileSystem);
    auto  wrapper= WrapperBuilder::BuildWrapper(KVWrapper::RocksDB);
   if(!wrapper.value()) {
       std::cout << "Error: cannot make the db wrapper!" << std::endl;
       exit(0);
   }
   auto performance =  std::make_shared<PerformanceMetrics>(wrapper.value());
   wrapper.value()->connect();
   while (newOperator.value()->HasNext()) {
       numberProducedOperation ++;

       wrapper.value()->operationsList.push_back(newOperator.value()->NextOperation());


       switch (static_cast<int>(wrapper.value()->operationsList.back())) {
           case 0:
               std::cout<< "get" <<std::endl;
               numberProducedGets ++;
               break;
           case 1:
               std::cout<< "put" <<std::endl;
               numberProducedPuts++;
               break;
           case 2:
               std::cout<< "merge" <<std::endl;
               numberProducedMerges ++;
               break;
           case 3:
               std::cout<< "delete" <<std::endl;
               numberProducedDeletes++;
               break;
           case 4:
               std::cout<< "scan" <<std::endl;
               numberProducedSeeks ++;
               break;
           case 5:
               std::cout<< "next" <<std::endl;
               numberProducedNexts ++;
               break;
       }


       wrapper.value()->keysList.push_back(newOperator.value()->NextKey());
       std::cout << "key: " << wrapper.value()->keysList.back()<< std::endl;
       wrapper.value()->valuesList.push_back(newOperator.value()->NextValue());
       std::cout << "value: " << wrapper.value()->valuesList.back() << std::endl;
       wrapper.value()->sleepTimes.push_back(newOperator.value()->NextSleepPeriod());
       std::cout << "number of done merges: "<<numberProducedMerges   << std::endl;
       std::cout << "number of done operations: "<<numberProducedOperation   << std::endl;
       /*
       if(numberProducedDeletes >  15) {
           break;
       }
        */
   }

    std::cout << "number of done operations: "<<numberProducedOperation   << std::endl;
    std::cout << "number of done merges: "<<numberProducedMerges   << std::endl;
    std::cout << "number of done puts: "<<numberProducedPuts  << std::endl;
    std::cout << "number of done gets: "<<numberProducedGets   << std::endl;
    std::cout << "number of done delets: "<<numberProducedDeletes   << std::endl;
    std::cout << "number of done  seeks: "<<numberProducedSeeks   << std::endl;
    std::cout << "number of done  nexts: "<<numberProducedNexts  << std::endl;

    std::cout << "Finished successfully :D" <<std::endl;

   /***
    * here we execute  the  generated commands on KV store
    */
    double throughput = wrapper.value()->runBatchThroughput();
    //wrapper.value()->dumpOperationsOnFile();
    wrapper.value()->disconnect();



    wrapper.value()->connect();
    wrapper.value()->runBatchLatency();
#endif

    /***
     * here we show the user the performance metrics
     */
    std::cout<< "******************** Results **********************" << std::endl;
    std::cout<< "Number of operations: " << numberProducedOperation << std::endl;
    std::cout<<"Throughput:" <<throughput * 1000000<<" opr/Sec" <<std::endl;
    std::cout<<"P99.99: "<< performance->ExpP99_99()<< " us" << std::endl;
    std::cout<<"P99.9: "<< performance->ExpP99_9()<< " us" <<std::endl;
    std::cout<<"P99: "<< performance->ExpP99()<<" us" << std::endl;
    std::cout<<"Max Response Time: "<< performance->ExpMax()<< " us" << std::endl;
    std::cout<<"Min Response Time: "<< performance->ExpMin()<< " us" << std::endl;
    std::cout<<"AVG Response Time: "<< performance->ExpAvg()<< " us" << std::endl;
    wrapper.value()->disconnect();

    return 0;
#endif

}


int main (int argc, char** argv) {
std::cout << "Hello from Gadget!" << std::endl;
    if(argc > 1) {
         drive_machine(argv[1]);
    } else {
        drive_machine("test.txt");
    }
#if 0
 // perform the tests
    auto tests = std::make_shared<Tests>();
    tests->RunAllTest();
    return 0;
#endif
}