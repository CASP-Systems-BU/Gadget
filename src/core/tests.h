// we do tests in this class for now. Ill move this  tests to
// google tests later

#ifndef GADGET_TESTS_H
#define GADGET_TESTS_H
#include <iostream>
#include <memory>
#include <optional>

#include "include/gadget/distributions/distributionParamters.h"
#include "include/gadget/distributions/serviceTime.h"
#include "include/gadget/distributions/value.h"
#include "include/gadget/distributions/keyPopularity.h"
#include "include/gadget/event/eventGenerator.h"
#include "include/gadget/event/eventGeneratorBuilder.h"
#include "include/gadget/event/eventGeneratorParameters.h"


class  Tests {
#if 0
public:
    Tests() {
        //  make an event generated that gets event from a file
        auto eventGeneratorParams = std::make_shared<EventGeneratorParameters>();
        eventGeneratorParams->eventFilePath = "events.txt";
        eventGenerator_ =  EventGeneratorBuilder::BuildEventGenerator(EventGenerator::File, eventGeneratorParams);
        // dump all generated events to a file
        wrapper_ = WrapperBuilder::BuildWrapper(KVWrapper::FileSystem);
    }
    void Reset() {
        auto eventGeneratorParams = std::make_shared<EventGeneratorParameters>();
        eventGeneratorParams->eventFilePath = "events.txt";
        eventGenerator_ =  EventGeneratorBuilder::BuildEventGenerator(EventGenerator::File, eventGeneratorParams);
        // dump all generated events to a file
        wrapper_ = WrapperBuilder::BuildWrapper(KVWrapper::FileSystem);
    }
    void RunAllTest() {
        /*
        TestTumblingAllWindowHolistic();
        Reset();
        TestTumblingAllWindowIncremental();
        Reset();
        TestTumblingKeyedWindowIncremental();
        Reset();
        TestTumblingKeyedWindowHolistic();
        Reset();
        TestSlidingAllWindowHolistic();
        Reset();
        TestSlidingAllWindowIncremental();
        Reset();
         */
        //TestSlidingKeyedWindowIncremental();
        Reset();
        //TestSlidingKeyedWindowHolistic();
        TestContinuousAggregation();
    }

    bool TestTumblingAllWindowHolistic() {
        //make the key size distribution
        auto keySizeDistributionParams = std::make_shared<DistributionParameters>();
        keySizeDistributionParams -> constantInt = 10; // size of the keys is 10 bytes
        // make  a fake parameter class for  key popularity distribution
        auto keyPopularityDistributionParams = std::make_shared<DistributionParameters>();
        // make the key distribution
        auto keyDistribution = KeyBuilder::BuildKey(KeyPopularity::TemporalIncreasing, keyPopularityDistributionParams , KeySize::ConstantDistrib, keySizeDistributionParams , 1000000);

        // make the value distribution
        auto valueDistributionParams = std::make_shared<DistributionParameters>();
        valueDistributionParams->constantInt = 1024;
        auto valueDistribution = ValueBuilder::BuildValue(Value::ConstantDistrib, valueDistributionParams);

        // make the service time distribution
        auto serviceTimeDistributionParams = std::make_shared<DistributionParameters>();
        serviceTimeDistributionParams->constantReal = 10;
        auto  serviceTimeDistribution = ServiceTimeBuilder::BuildServiceTime(ServiceTime::ConstantDistrib, serviceTimeDistributionParams);

       // check if  distribution are valid
       assert(serviceTimeDistribution.has_value());
       assert(valueDistribution.has_value());
       assert(keyDistribution.has_value());


       // make the operator parameters
        auto operatorParams = std::make_shared<OperatorParameters>();
        operatorParams->windowLength = 5;
        operatorParams->allowedLateness = false;
        operatorParams->latenessLength = 0;
        operatorParams->slidingLength = 1;
        operatorParams->eventGenerator= eventGenerator_.value();
        operatorParams->serviceTimeDistribution = serviceTimeDistribution.value();
        operatorParams->keysDistribution = keyDistribution.value();
        operatorParams->valuesDistribution = valueDistribution.value();
        operatorParams->operatorType = Operator::OperatorWindowTumblingAllHolistic;

        // make the operator
        auto newOperator = OperatorBuilder::BuildOperator(operatorParams);

        // check if operator is valid
        assert(newOperator.has_value());





        // used to check the number of generated operations is right
        uint64_t  numberProducedOperations = 0;
        uint64_t  numberProducedGetOperations = 0;
        uint64_t  numberProducedPutOperations = 0;
        uint64_t  numberProducedDeleteOperations = 0;
        uint64_t  numberProducedMergeOperations = 0;

        // run the operator and check if it operates acts as expected
        while (newOperator.value()->HasNext() ) {
            numberProducedOperations ++;

            wrapper_.value()->operationsList.push_back(newOperator.value()->NextOperation());

            
            switch (static_cast<int>(wrapper_.value()->operationsList.back())) {
                case 0:
                    std::cout<< "get" <<std::endl;
                    numberProducedGetOperations ++;
                    break;
                case 1:
                    std::cout<< "put" <<std::endl;
                    numberProducedPutOperations ++;
                    break;
                case 2:
                    std::cout<< "merge" <<std::endl;
                    numberProducedMergeOperations ++;
                    break;
                case 3:
                    std::cout<< "delete" <<std::endl;
                    numberProducedDeleteOperations ++;
                    break;
            }

          
            wrapper_.value()->keysList.push_back(newOperator.value()->NextKey());
            //std::cout << "key: " << wrapper_.value()->keysList.back()<< std::endl; // print the window key
            wrapper_.value()->valuesList.push_back(newOperator.value()->NextValue());
            // std::cout << "value: " << wrapper_.value()->valuesList.back() << std::endl;
            wrapper_.value()->sleepTimes.push_back(newOperator.value()->NextSleepPeriod());
           // std::cout << "number of done operations: "<<numberProducedOperation <<std::endl;
        }

        // check the value size
        assert (wrapper_.value()->valuesList.back().length() == 1024);

        // print the  last generated  value
        std::cout << "The last generated value is: " << wrapper_.value()->valuesList.back() << std::endl;


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() == 10);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;

        // check if the keys are increasing- print them
        for (auto entry : wrapper_.value()->keysList) {
            std::cout << entry <<std::endl;
        }

        // check the last sleep time
        std::cout << "The last generated sleep  time: " << wrapper_.value()->sleepTimes.back() << std::endl;
        
        // check  the operations  statistics
        std::cout<< numberProducedOperations << std::endl;
        assert(numberProducedOperations == 86);
        assert(numberProducedGetOperations  == 3 );
        assert(numberProducedPutOperations  == 0);
        assert(numberProducedDeleteOperations  == 3);
        assert(numberProducedMergeOperations  == 80);

        std::cout << "TestTumblingAllWindowIncremental: All tests are passed";
        
       }
    bool TestTumblingAllWindowIncremental() {
        //make the key size distribution
        auto keySizeDistributionParams = std::make_shared<DistributionParameters>();
        keySizeDistributionParams -> constantInt = 10; // size of the keys is 10 bytes
        // make  a fake parameter class for  key popularity distribution
        auto keyPopularityDistributionParams = std::make_shared<DistributionParameters>();
        keyPopularityDistributionParams->s = 1;
        // make the key distribution
        auto keyDistribution = KeyBuilder::BuildKey(KeyPopularity::TemporalIncreasing, keyPopularityDistributionParams , KeySize::ConstantDistrib, keySizeDistributionParams , 1000000);

        // make the value distribution
        auto valueDistributionParams = std::make_shared<DistributionParameters>();
        valueDistributionParams->constantInt = 1024;
        auto valueDistribution = ValueBuilder::BuildValue(Value::ConstantDistrib, valueDistributionParams);

        // make the service time distribution
        auto serviceTimeDistributionParams = std::make_shared<DistributionParameters>();
        serviceTimeDistributionParams->lambda = 0.2;
        auto  serviceTimeDistribution = ServiceTimeBuilder::BuildServiceTime(ServiceTime::ExponentialDistrib, serviceTimeDistributionParams);

        // check if  distribution are valid
        assert(serviceTimeDistribution.has_value());
        assert(valueDistribution.has_value());
        assert(keyDistribution.has_value());


        // make the operator parameters
        auto operatorParams = std::make_shared<OperatorParameters>();
        operatorParams->windowLength = 5;
        operatorParams->allowedLateness = false;
        operatorParams->latenessLength = 0;
        operatorParams->slidingLength = 1;
        operatorParams->eventGenerator= eventGenerator_.value();
        operatorParams->serviceTimeDistribution = serviceTimeDistribution.value();
        operatorParams->keysDistribution = keyDistribution.value();
        operatorParams->valuesDistribution = valueDistribution.value();
        operatorParams->operatorType = Operator::OperatorWindowTumblingAllIncremental;

        // make the operator
        auto newOperator = OperatorBuilder::BuildOperator(operatorParams);

        // check if operator is valid
        assert(newOperator.has_value());





        // used to check the number of generated operations is right
        uint64_t  numberProducedOperations = 0;
        uint64_t  numberProducedGetOperations = 0;
        uint64_t  numberProducedPutOperations = 0;
        uint64_t  numberProducedDeleteOperations = 0;
        uint64_t  numberProducedMergeOperations = 0;

        // run the operator and check if it operates acts as expected
        while (newOperator.value()->HasNext() ) {
            numberProducedOperations ++;

            wrapper_.value()->operationsList.push_back(newOperator.value()->NextOperation());


            switch (static_cast<int>(wrapper_.value()->operationsList.back())) {
                case 0:
                    std::cout<< "get" <<std::endl;
                    numberProducedGetOperations ++;
                    break;
                case 1:
                    std::cout<< "put" <<std::endl;
                    numberProducedPutOperations ++;
                    break;
                case 2:
                    std::cout<< "merge" <<std::endl;
                    numberProducedMergeOperations ++;
                    break;
                case 3:
                    std::cout<< "delete" <<std::endl;
                    numberProducedDeleteOperations ++;
                    break;
            }


            wrapper_.value()->keysList.push_back(newOperator.value()->NextKey());
            //std::cout << "key: " << wrapper_.value()->keysList.back()<< std::endl; // print the window key
            wrapper_.value()->valuesList.push_back(newOperator.value()->NextValue());
            // std::cout << "value: " << wrapper_.value()->valuesList.back() << std::endl;
            wrapper_.value()->sleepTimes.push_back(newOperator.value()->NextSleepPeriod());
            // std::cout << "number of done operations: "<<numberProducedOperation <<std::endl;
        }


        // check  the operations  statistics
        std::cout<< numberProducedOperations << std::endl;
        assert(numberProducedOperations == 166);
        assert(numberProducedGetOperations  == 83 );
        assert(numberProducedPutOperations  == 80);
        assert(numberProducedDeleteOperations  == 3);
        assert(numberProducedMergeOperations  == 0);

        // check the value size
        assert (wrapper_.value()->valuesList.back().length() == 1024);

        // print the  last generated  value
        std::cout << "The last generated value is: " << wrapper_.value()->valuesList.back() << std::endl;


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() > 0);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;

        // check if the keys are increasing- print them
        //for (auto entry : wrapper_.value()->keysList) {
        //    std::cout << entry <<std::endl;
        //}

         //print the generated sleep times
         for (auto entry : wrapper_.value()->sleepTimes) {
           std::cout << entry <<std::endl;
        }

        // check the last sleep time
        std::cout << "The last generated sleep  time: " << wrapper_.value()->sleepTimes.back() << std::endl;



        std::cout << "TestTumblingAllWindowIncremental: All tests are passed";

    }
    bool TestTumblingKeyedWindowIncremental() {
        //make the key size distribution
        auto keySizeDistributionParams = std::make_shared<DistributionParameters>();
        keySizeDistributionParams -> constantInt = 10; // size of the keys is 10 bytes
        // make  a fake parameter class for  key popularity distribution
        auto keyPopularityDistributionParams = std::make_shared<DistributionParameters>();
        keyPopularityDistributionParams->s = 1;
        // make the key distribution
        auto keyDistribution = KeyBuilder::BuildKey(KeyPopularity::TemporalIncreasing, keyPopularityDistributionParams , KeySize::ConstantDistrib, keySizeDistributionParams , 1000000);

        // make the value distribution
        auto valueDistributionParams = std::make_shared<DistributionParameters>();
        valueDistributionParams->constantInt = 1024;
        auto valueDistribution = ValueBuilder::BuildValue(Value::ConstantDistrib, valueDistributionParams);

        // make the service time distribution
        auto serviceTimeDistributionParams = std::make_shared<DistributionParameters>();
        serviceTimeDistributionParams->lambda = 0.2;
        auto  serviceTimeDistribution = ServiceTimeBuilder::BuildServiceTime(ServiceTime::ExponentialDistrib, serviceTimeDistributionParams);

        // check if  distribution are valid
        assert(serviceTimeDistribution.has_value());
        assert(valueDistribution.has_value());
        assert(keyDistribution.has_value());


        // make the operator parameters
        auto operatorParams = std::make_shared<OperatorParameters>();
        operatorParams->windowLength = 5;
        operatorParams->allowedLateness = false;
        operatorParams->latenessLength = 0;
        operatorParams->slidingLength = 1;
        operatorParams->eventGenerator= eventGenerator_.value();
        operatorParams->serviceTimeDistribution = serviceTimeDistribution.value();
        operatorParams->keysDistribution = keyDistribution.value();
        operatorParams->valuesDistribution = valueDistribution.value();
        operatorParams->operatorType = Operator::OperatorWindowTumblingKeyedIncremental;

        // make the operator
        auto newOperator = OperatorBuilder::BuildOperator(operatorParams);

        // check if operator is valid
        assert(newOperator.has_value());





        // used to check the number of generated operations is right
        uint64_t  numberProducedOperations = 0;
        uint64_t  numberProducedGetOperations = 0;
        uint64_t  numberProducedPutOperations = 0;
        uint64_t  numberProducedDeleteOperations = 0;
        uint64_t  numberProducedMergeOperations = 0;

        // run the operator and check if it operates acts as expected
        while (newOperator.value()->HasNext() ) {
            numberProducedOperations ++;

            wrapper_.value()->operationsList.push_back(newOperator.value()->NextOperation());


            switch (static_cast<int>(wrapper_.value()->operationsList.back())) {
                case 0:
                    std::cout<< "get" <<std::endl;
                    numberProducedGetOperations ++;
                    break;
                case 1:
                    std::cout<< "put" <<std::endl;
                    numberProducedPutOperations ++;
                    break;
                case 2:
                    std::cout<< "merge" <<std::endl;
                    numberProducedMergeOperations ++;
                    break;
                case 3:
                    std::cout<< "delete" <<std::endl;
                    numberProducedDeleteOperations ++;
                    break;
                default:
                    std::cout<< "unknown" <<std::endl;
            }


            wrapper_.value()->keysList.push_back(newOperator.value()->NextKey());
            //std::cout << "key: " << wrapper_.value()->keysList.back()<< std::endl; // print the window key
            wrapper_.value()->valuesList.push_back(newOperator.value()->NextValue());
            // std::cout << "value: " << wrapper_.value()->valuesList.back() << std::endl;
            wrapper_.value()->sleepTimes.push_back(newOperator.value()->NextSleepPeriod());
            // std::cout << "number of done operations: "<<numberProducedOperation <<std::endl;
        }


        // check  the operations  statistics
        std::cout<< numberProducedOperations << std::endl;
        assert(numberProducedOperations == 212);
        assert(numberProducedGetOperations  == (80+26) );
        assert(numberProducedPutOperations  == 80  );
        assert(numberProducedDeleteOperations  == 26);
        assert(numberProducedMergeOperations  == 0);

        // check the value size
        assert (wrapper_.value()->valuesList.back().length() == 1024);

        // print the  last generated  value
        std::cout << "The last generated value is: " << wrapper_.value()->valuesList.back() << std::endl;


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() > 0);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;

        // check if the keys are increasing- print them
        //for (auto entry : wrapper_.value()->keysList) {
        //    std::cout << entry <<std::endl;
        //}

        //print the generated sleep times
        for (auto entry : wrapper_.value()->sleepTimes) {
            std::cout << entry <<std::endl;
        }

        // check the last sleep time
        std::cout << "The last generated sleep  time: " << wrapper_.value()->sleepTimes.back() << std::endl;



        std::cout << "TestTumblingKeyedWindowIncremental: All tests are passed";

    }
    bool TestTumblingKeyedWindowHolistic() {
        //make the key size distribution
        auto keySizeDistributionParams = std::make_shared<DistributionParameters>();
        keySizeDistributionParams -> constantInt = 10; // size of the keys is 10 bytes
        // make  a fake parameter class for  key popularity distribution
        auto keyPopularityDistributionParams = std::make_shared<DistributionParameters>();
        keyPopularityDistributionParams->s = 1;
        // make the key distribution
        auto keyDistribution = KeyBuilder::BuildKey(KeyPopularity::TemporalIncreasing, keyPopularityDistributionParams , KeySize::ConstantDistrib, keySizeDistributionParams , 1000000);

        // make the value distribution
        auto valueDistributionParams = std::make_shared<DistributionParameters>();
        valueDistributionParams->constantInt = 1024;
        auto valueDistribution = ValueBuilder::BuildValue(Value::ConstantDistrib, valueDistributionParams);

        // make the service time distribution
        auto serviceTimeDistributionParams = std::make_shared<DistributionParameters>();
        serviceTimeDistributionParams->lambda = 0.2;
        auto  serviceTimeDistribution = ServiceTimeBuilder::BuildServiceTime(ServiceTime::ExponentialDistrib, serviceTimeDistributionParams);

        // check if  distribution are valid
        assert(serviceTimeDistribution.has_value());
        assert(valueDistribution.has_value());
        assert(keyDistribution.has_value());


        // make the operator parameters
        auto operatorParams = std::make_shared<OperatorParameters>();
        operatorParams->windowLength = 5;
        operatorParams->allowedLateness = false;
        operatorParams->latenessLength = 0;
        operatorParams->slidingLength = 1;
        operatorParams->eventGenerator= eventGenerator_.value();
        operatorParams->serviceTimeDistribution = serviceTimeDistribution.value();
        operatorParams->keysDistribution = keyDistribution.value();
        operatorParams->valuesDistribution = valueDistribution.value();
        operatorParams->operatorType = Operator::OperatorWindowTumblingKeyedHolistic;

        // make the operator
        auto newOperator = OperatorBuilder::BuildOperator(operatorParams);

        // check if operator is valid
        assert(newOperator.has_value());





        // used to check the number of generated operations is right
        uint64_t  numberProducedOperations = 0;
        uint64_t  numberProducedGetOperations = 0;
        uint64_t  numberProducedPutOperations = 0;
        uint64_t  numberProducedDeleteOperations = 0;
        uint64_t  numberProducedMergeOperations = 0;

        // run the operator and check if it operates acts as expected
        while (newOperator.value()->HasNext() ) {
            numberProducedOperations ++;

            wrapper_.value()->operationsList.push_back(newOperator.value()->NextOperation());


            switch (static_cast<int>(wrapper_.value()->operationsList.back())) {
                case 0:
                    std::cout<< "get" <<std::endl;
                    numberProducedGetOperations ++;
                    break;
                case 1:
                    std::cout<< "put" <<std::endl;
                    numberProducedPutOperations ++;
                    break;
                case 2:
                    std::cout<< "merge" <<std::endl;
                    numberProducedMergeOperations ++;
                    break;
                case 3:
                    std::cout<< "delete" <<std::endl;
                    numberProducedDeleteOperations ++;
                    break;
                default:
                    std::cout<< "unknown" <<std::endl;
            }


            wrapper_.value()->keysList.push_back(newOperator.value()->NextKey());
            //std::cout << "key: " << wrapper_.value()->keysList.back()<< std::endl; // print the window key
            wrapper_.value()->valuesList.push_back(newOperator.value()->NextValue());
            // std::cout << "value: " << wrapper_.value()->valuesList.back() << std::endl;
            wrapper_.value()->sleepTimes.push_back(newOperator.value()->NextSleepPeriod());
            // std::cout << "number of done operations: "<<numberProducedOperation <<std::endl;
        }


        // check  the operations  statistics
        std::cout<< numberProducedOperations << std::endl;
        assert(numberProducedOperations == 132);
        assert(numberProducedGetOperations  == 26 );
        assert(numberProducedPutOperations  == 0  );
        assert(numberProducedDeleteOperations  == 26);
        assert(numberProducedMergeOperations  == 80);

        // check the value size
        assert (wrapper_.value()->valuesList.back().length() == 1024);

        // print the  last generated  value
        std::cout << "The last generated value is: " << wrapper_.value()->valuesList.back() << std::endl;


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() > 0);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;

        // check if the keys are increasing- print them
        //for (auto entry : wrapper_.value()->keysList) {
        //    std::cout << entry <<std::endl;
        //}

        //print the generated sleep times
        for (auto entry : wrapper_.value()->sleepTimes) {
            std::cout << entry <<std::endl;
        }

        // check the last sleep time
        std::cout << "The last generated sleep  time: " << wrapper_.value()->sleepTimes.back() << std::endl;



        std::cout << "TestTumblingKeyedWindowIncremental: All tests are passed";

    }


    bool TestSlidingAllWindowHolistic() {
        //make the key size distribution
        auto keySizeDistributionParams = std::make_shared<DistributionParameters>();
        keySizeDistributionParams -> constantInt = 15; // size of the keys is 10 bytes
        // make  a fake parameter class for  key popularity distribution
        auto keyPopularityDistributionParams = std::make_shared<DistributionParameters>();
        // make the key distribution
        auto keyDistribution = KeyBuilder::BuildKey(KeyPopularity::TemporalIncreasing, keyPopularityDistributionParams , KeySize::ConstantDistrib, keySizeDistributionParams , 1000000);

        // make the value distribution
        auto valueDistributionParams = std::make_shared<DistributionParameters>();
        valueDistributionParams->constantInt = 1024;
        auto valueDistribution = ValueBuilder::BuildValue(Value::ConstantDistrib, valueDistributionParams);

        // make the service time distribution
        auto serviceTimeDistributionParams = std::make_shared<DistributionParameters>();
        serviceTimeDistributionParams->constantReal = 10;
        auto  serviceTimeDistribution = ServiceTimeBuilder::BuildServiceTime(ServiceTime::ConstantDistrib, serviceTimeDistributionParams);

        // check if  distribution are valid
        assert(serviceTimeDistribution.has_value());
        assert(valueDistribution.has_value());
        assert(keyDistribution.has_value());


        // make the operator parameters
        auto operatorParams = std::make_shared<OperatorParameters>();
        operatorParams->windowLength = 5;
        operatorParams->allowedLateness = false;
        operatorParams->latenessLength = 0;
        operatorParams->slidingLength = 1;
        operatorParams->eventGenerator= eventGenerator_.value();
        operatorParams->serviceTimeDistribution = serviceTimeDistribution.value();
        operatorParams->keysDistribution = keyDistribution.value();
        operatorParams->valuesDistribution = valueDistribution.value();
        operatorParams->operatorType = Operator::OperatorWindowSlidingAllHolistic;

        // make the operator
        auto newOperator = OperatorBuilder::BuildOperator(operatorParams);

        // check if operator is valid
        assert(newOperator.has_value());





        // used to check the number of generated operations is right
        uint64_t  numberProducedOperations = 0;
        uint64_t  numberProducedGetOperations = 0;
        uint64_t  numberProducedPutOperations = 0;
        uint64_t  numberProducedDeleteOperations = 0;
        uint64_t  numberProducedMergeOperations = 0;

        // run the operator and check if it operates acts as expected
        while (newOperator.value()->HasNext() ) {
            numberProducedOperations ++;

            wrapper_.value()->operationsList.push_back(newOperator.value()->NextOperation());


            switch (static_cast<int>(wrapper_.value()->operationsList.back())) {
                case 0:
                    std::cout<< "get" <<std::endl;
                    numberProducedGetOperations ++;
                    break;
                case 1:
                    std::cout<< "put" <<std::endl;
                    numberProducedPutOperations ++;
                    break;
                case 2:
                    std::cout<< "merge" <<std::endl;
                    numberProducedMergeOperations ++;
                    break;
                case 3:
                    std::cout<< "delete" <<std::endl;
                    numberProducedDeleteOperations ++;
                    break;
            }




            wrapper_.value()->keysList.push_back(newOperator.value()->NextKey());
            //std::cout << "key: " << wrapper_.value()->keysList.back()<< std::endl; // print the window key
            wrapper_.value()->valuesList.push_back(newOperator.value()->NextValue());
            // std::cout << "value: " << wrapper_.value()->valuesList.back() << std::endl;
            wrapper_.value()->sleepTimes.push_back(newOperator.value()->NextSleepPeriod());
            // std::cout << "number of done operations: "<<numberProducedOperation <<std::endl;
        }


        // check if the keys are increasing- print them
        for (auto entry : wrapper_.value()->keysList) {
            std::cout << entry <<std::endl;
        }


        // check the key size of the last generated key
                assert (wrapper_.value()->keysList.back().length() == 15);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;


        // check  the operations  statistics
        std::cout<< numberProducedOperations << std::endl;
        assert(numberProducedOperations == 347);
        assert(numberProducedGetOperations  == 12 );
        assert(numberProducedPutOperations  == 0);
        assert(numberProducedDeleteOperations  == 12);
        assert(numberProducedMergeOperations  == 323);

        // check the value size
        assert (wrapper_.value()->valuesList.back().length() == 1024);

        // print the  last generated  value
        std::cout << "The last generated value is: " << wrapper_.value()->valuesList.back() << std::endl;


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() == 15);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;



        // check the last sleep time
        std::cout << "The last generated sleep  time: " << wrapper_.value()->sleepTimes.back() << std::endl;


        std::cout << "TestSlidingAllWindowHolistic: All tests are passed";

    }
    bool TestSlidingAllWindowIncremental() {
        //make the key size distribution
        auto keySizeDistributionParams = std::make_shared<DistributionParameters>();
        keySizeDistributionParams -> constantInt = 15; // size of the keys is 10 bytes
        // make  a fake parameter class for  key popularity distribution
        auto keyPopularityDistributionParams = std::make_shared<DistributionParameters>();
        // make the key distribution
        auto keyDistribution = KeyBuilder::BuildKey(KeyPopularity::TemporalIncreasing, keyPopularityDistributionParams , KeySize::ConstantDistrib, keySizeDistributionParams , 1000000);

        // make the value distribution
        auto valueDistributionParams = std::make_shared<DistributionParameters>();
        valueDistributionParams->constantInt = 1024;
        auto valueDistribution = ValueBuilder::BuildValue(Value::ConstantDistrib, valueDistributionParams);

        // make the service time distribution
        auto serviceTimeDistributionParams = std::make_shared<DistributionParameters>();
        serviceTimeDistributionParams->constantReal = 10;
        auto  serviceTimeDistribution = ServiceTimeBuilder::BuildServiceTime(ServiceTime::ConstantDistrib, serviceTimeDistributionParams);

        // check if  distribution are valid
        assert(serviceTimeDistribution.has_value());
        assert(valueDistribution.has_value());
        assert(keyDistribution.has_value());


        // make the operator parameters
        auto operatorParams = std::make_shared<OperatorParameters>();
        operatorParams->windowLength = 5;
        operatorParams->allowedLateness = false;
        operatorParams->latenessLength = 0;
        operatorParams->slidingLength = 1;
        operatorParams->eventGenerator= eventGenerator_.value();
        operatorParams->serviceTimeDistribution = serviceTimeDistribution.value();
        operatorParams->keysDistribution = keyDistribution.value();
        operatorParams->valuesDistribution = valueDistribution.value();
        operatorParams->operatorType = Operator::OperatorWindowSlidingAllIncremental;

        // make the operator
        auto newOperator = OperatorBuilder::BuildOperator(operatorParams);

        // check if operator is valid
        assert(newOperator.has_value());





        // used to check the number of generated operations is right
        uint64_t  numberProducedOperations = 0;
        uint64_t  numberProducedGetOperations = 0;
        uint64_t  numberProducedPutOperations = 0;
        uint64_t  numberProducedDeleteOperations = 0;
        uint64_t  numberProducedMergeOperations = 0;

        // run the operator and check if it operates acts as expected
        while (newOperator.value()->HasNext() ) {
            numberProducedOperations ++;

            wrapper_.value()->operationsList.push_back(newOperator.value()->NextOperation());


            switch (static_cast<int>(wrapper_.value()->operationsList.back())) {
                case 0:
                    std::cout<< "get" <<std::endl;
                    numberProducedGetOperations ++;
                    break;
                case 1:
                    std::cout<< "put" <<std::endl;
                    numberProducedPutOperations ++;
                    break;
                case 2:
                    std::cout<< "merge" <<std::endl;
                    numberProducedMergeOperations ++;
                    break;
                case 3:
                    std::cout<< "delete" <<std::endl;
                    numberProducedDeleteOperations ++;
                    break;
            }




            wrapper_.value()->keysList.push_back(newOperator.value()->NextKey());
            //std::cout << "key: " << wrapper_.value()->keysList.back()<< std::endl; // print the window key
            wrapper_.value()->valuesList.push_back(newOperator.value()->NextValue());
            // std::cout << "value: " << wrapper_.value()->valuesList.back() << std::endl;
            wrapper_.value()->sleepTimes.push_back(newOperator.value()->NextSleepPeriod());
            // std::cout << "number of done operations: "<<numberProducedOperation <<std::endl;
        }


        // check if the keys are increasing- print them
        for (auto entry : wrapper_.value()->keysList) {
            std::cout << entry <<std::endl;
        }


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() == 15);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;


        // check  the operations  statistics
        std::cout<< numberProducedOperations << std::endl;
        assert(numberProducedOperations == 670);
        assert(numberProducedGetOperations  == 12  + 323);
        assert(numberProducedPutOperations  == 323);
        assert(numberProducedDeleteOperations  == 12);
        assert(numberProducedMergeOperations  == 0);

        // check the value size
        assert (wrapper_.value()->valuesList.back().length() == 1024);

        // print the  last generated  value
        std::cout << "The last generated value is: " << wrapper_.value()->valuesList.back() << std::endl;


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() == 15);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;



        // check the last sleep time
        std::cout << "The last generated sleep  time: " << wrapper_.value()->sleepTimes.back() << std::endl;


        std::cout << "TestSlidingAllWindowIncremental: All tests are passed";

    }
    bool TestSlidingKeyedWindowIncremental() {
        //make the key size distribution
        auto keySizeDistributionParams = std::make_shared<DistributionParameters>();
        keySizeDistributionParams -> constantInt = 15; // size of the keys is 10 bytes
        // make  a fake parameter class for  key popularity distribution
        auto keyPopularityDistributionParams = std::make_shared<DistributionParameters>();
        // make the key distribution
        auto keyDistribution = KeyBuilder::BuildKey(KeyPopularity::TemporalIncreasing, keyPopularityDistributionParams , KeySize::ConstantDistrib, keySizeDistributionParams , 1000000);

        // make the value distribution
        auto valueDistributionParams = std::make_shared<DistributionParameters>();
        valueDistributionParams->constantInt = 1024;
        auto valueDistribution = ValueBuilder::BuildValue(Value::ConstantDistrib, valueDistributionParams);

        // make the service time distribution
        auto serviceTimeDistributionParams = std::make_shared<DistributionParameters>();
        serviceTimeDistributionParams->constantReal = 10;
        auto  serviceTimeDistribution = ServiceTimeBuilder::BuildServiceTime(ServiceTime::ConstantDistrib, serviceTimeDistributionParams);

        // check if  distribution are valid
        assert(serviceTimeDistribution.has_value());
        assert(valueDistribution.has_value());
        assert(keyDistribution.has_value());


        // make the operator parameters
        auto operatorParams = std::make_shared<OperatorParameters>();
        operatorParams->windowLength = 5;
        operatorParams->allowedLateness = false;
        operatorParams->latenessLength = 0;
        operatorParams->slidingLength = 1;
        operatorParams->eventGenerator= eventGenerator_.value();
        operatorParams->serviceTimeDistribution = serviceTimeDistribution.value();
        operatorParams->keysDistribution = keyDistribution.value();
        operatorParams->valuesDistribution = valueDistribution.value();
        operatorParams->operatorType = Operator::OperatorWindowSlidingKeyedIncremental;

        // make the operator
        auto newOperator = OperatorBuilder::BuildOperator(operatorParams);

        // check if operator is valid
        assert(newOperator.has_value());





        // used to check the number of generated operations is right
        uint64_t  numberProducedOperations = 0;
        uint64_t  numberProducedGetOperations = 0;
        uint64_t  numberProducedPutOperations = 0;
        uint64_t  numberProducedDeleteOperations = 0;
        uint64_t  numberProducedMergeOperations = 0;

        // run the operator and check if it operates acts as expected
        while (newOperator.value()->HasNext() ) {
            numberProducedOperations ++;

            wrapper_.value()->operationsList.push_back(newOperator.value()->NextOperation());


            switch (static_cast<int>(wrapper_.value()->operationsList.back())) {
                case 0:
                    std::cout<< "get" <<std::endl;
                    numberProducedGetOperations ++;
                    break;
                case 1:
                    std::cout<< "put" <<std::endl;
                    numberProducedPutOperations ++;
                    break;
                case 2:
                    std::cout<< "merge" <<std::endl;
                    numberProducedMergeOperations ++;
                    break;
                case 3:
                    std::cout<< "delete" <<std::endl;
                    numberProducedDeleteOperations ++;
                    break;
            }




            wrapper_.value()->keysList.push_back(newOperator.value()->NextKey());
            //std::cout << "key: " << wrapper_.value()->keysList.back()<< std::endl; // print the window key
            wrapper_.value()->valuesList.push_back(newOperator.value()->NextValue());
            // std::cout << "value: " << wrapper_.value()->valuesList.back() << std::endl;
            wrapper_.value()->sleepTimes.push_back(newOperator.value()->NextSleepPeriod());
            // std::cout << "number of done operations: "<<numberProducedOperation <<std::endl;
        }


        // check if the keys are increasing- print them
        for (auto entry : wrapper_.value()->keysList) {
            std::cout << entry <<std::endl;
        }


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() == 15);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;


        // check  the operations  statistics
        std::cout<< numberProducedOperations << std::endl;
        assert(numberProducedPutOperations  == 323);
        assert(numberProducedOperations == 646+ 24+ 24);
        assert(numberProducedGetOperations  == 24  + 323);
        assert(numberProducedDeleteOperations  == 24);
        assert(numberProducedMergeOperations  == 0);

        // check the value size
        assert (wrapper_.value()->valuesList.back().length() == 1024);

        // print the  last generated  value
        std::cout << "The last generated value is: " << wrapper_.value()->valuesList.back() << std::endl;


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() == 15);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;



        // check the last sleep time
        std::cout << "The last generated sleep  time: " << wrapper_.value()->sleepTimes.back() << std::endl;


        std::cout << "TestSlidingAllWindowIncremental: All tests are passed";

    }
    bool TestSlidingKeyedWindowHolistic() {
        //make the key size distribution
        auto keySizeDistributionParams = std::make_shared<DistributionParameters>();
        keySizeDistributionParams -> constantInt = 15; // size of the keys is 10 bytes
        // make  a fake parameter class for  key popularity distribution
        auto keyPopularityDistributionParams = std::make_shared<DistributionParameters>();
        // make the key distribution
        auto keyDistribution = KeyBuilder::BuildKey(KeyPopularity::TemporalIncreasing, keyPopularityDistributionParams , KeySize::ConstantDistrib, keySizeDistributionParams , 1000000);

        // make the value distribution
        auto valueDistributionParams = std::make_shared<DistributionParameters>();
        valueDistributionParams->constantInt = 1024;
        auto valueDistribution = ValueBuilder::BuildValue(Value::ConstantDistrib, valueDistributionParams);

        // make the service time distribution
        auto serviceTimeDistributionParams = std::make_shared<DistributionParameters>();
        serviceTimeDistributionParams->constantReal = 10;
        auto  serviceTimeDistribution = ServiceTimeBuilder::BuildServiceTime(ServiceTime::ConstantDistrib, serviceTimeDistributionParams);

        // check if  distribution are valid
        assert(serviceTimeDistribution.has_value());
        assert(valueDistribution.has_value());
        assert(keyDistribution.has_value());


        // make the operator parameters
        auto operatorParams = std::make_shared<OperatorParameters>();
        operatorParams->windowLength = 5;
        operatorParams->allowedLateness = false;
        operatorParams->latenessLength = 0;
        operatorParams->slidingLength = 1;
        operatorParams->eventGenerator= eventGenerator_.value();
        operatorParams->serviceTimeDistribution = serviceTimeDistribution.value();
        operatorParams->keysDistribution = keyDistribution.value();
        operatorParams->valuesDistribution = valueDistribution.value();
        operatorParams->operatorType = Operator::OperatorWindowSlidingKeyedHolistic;

        // make the operator
        auto newOperator = OperatorBuilder::BuildOperator(operatorParams);

        // check if operator is valid
        assert(newOperator.has_value());





        // used to check the number of generated operations is right
        uint64_t  numberProducedOperations = 0;
        uint64_t  numberProducedGetOperations = 0;
        uint64_t  numberProducedPutOperations = 0;
        uint64_t  numberProducedDeleteOperations = 0;
        uint64_t  numberProducedMergeOperations = 0;

        // run the operator and check if it operates acts as expected
        while (newOperator.value()->HasNext() ) {
            numberProducedOperations ++;

            wrapper_.value()->operationsList.push_back(newOperator.value()->NextOperation());


            switch (static_cast<int>(wrapper_.value()->operationsList.back())) {
                case 0:
                    std::cout<< "get" <<std::endl;
                    numberProducedGetOperations ++;
                    break;
                case 1:
                    std::cout<< "put" <<std::endl;
                    numberProducedPutOperations ++;
                    break;
                case 2:
                    std::cout<< "merge" <<std::endl;
                    numberProducedMergeOperations ++;
                    break;
                case 3:
                    std::cout<< "delete" <<std::endl;
                    numberProducedDeleteOperations ++;
                    break;
            }




            wrapper_.value()->keysList.push_back(newOperator.value()->NextKey());
            //std::cout << "key: " << wrapper_.value()->keysList.back()<< std::endl; // print the window key
            wrapper_.value()->valuesList.push_back(newOperator.value()->NextValue());
            // std::cout << "value: " << wrapper_.value()->valuesList.back() << std::endl;
            wrapper_.value()->sleepTimes.push_back(newOperator.value()->NextSleepPeriod());
            // std::cout << "number of done operations: "<<numberProducedOperation <<std::endl;
        }


        // check if the keys are increasing- print them
        for (auto entry : wrapper_.value()->keysList) {
            std::cout << entry <<std::endl;
        }


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() == 15);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;


        // check  the operations  statistics
        std::cout<< numberProducedOperations << std::endl;
        assert(numberProducedPutOperations  == 0);
        assert(numberProducedOperations == 323 + 24 + 24);
        assert(numberProducedGetOperations  == 24  );
        assert(numberProducedDeleteOperations  == 24);
        assert(numberProducedMergeOperations  == 323);

        // check the value size
        assert (wrapper_.value()->valuesList.back().length() == 1024);

        // print the  last generated  value
        std::cout << "The last generated value is: " << wrapper_.value()->valuesList.back() << std::endl;


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() == 15);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;



        // check the last sleep time
        std::cout << "The last generated sleep  time: " << wrapper_.value()->sleepTimes.back() << std::endl;


        std::cout << "TestSlidingAllWindowIncremental: All tests are passed";

    }

    bool TestContinuousAggregation() {
        //make the key size distribution
        auto keySizeDistributionParams = std::make_shared<DistributionParameters>();
        keySizeDistributionParams -> constantInt = 15; // size of the keys is 10 bytes
        // make  a fake parameter class for  key popularity distribution
        auto keyPopularityDistributionParams = std::make_shared<DistributionParameters>();
        // make the key distribution
        auto keyDistribution = KeyBuilder::BuildKey(KeyPopularity::TemporalIncreasing, keyPopularityDistributionParams , KeySize::ConstantDistrib, keySizeDistributionParams , 1000000);

        // make the value distribution
        auto valueDistributionParams = std::make_shared<DistributionParameters>();
        valueDistributionParams->constantInt = 1024;
        auto valueDistribution = ValueBuilder::BuildValue(Value::ConstantDistrib, valueDistributionParams);

        // make the service time distribution
        auto serviceTimeDistributionParams = std::make_shared<DistributionParameters>();
        serviceTimeDistributionParams->constantReal = 10;
        auto  serviceTimeDistribution = ServiceTimeBuilder::BuildServiceTime(ServiceTime::ConstantDistrib, serviceTimeDistributionParams);

        // check if  distribution are valid
        assert(serviceTimeDistribution.has_value());
        assert(valueDistribution.has_value());
        assert(keyDistribution.has_value());


        // make the operator parameters
        auto operatorParams = std::make_shared<OperatorParameters>();
        operatorParams->windowLength = 5;
        operatorParams->allowedLateness = false;
        operatorParams->latenessLength = 0;
        operatorParams->slidingLength = 1;
        operatorParams->eventGenerator= eventGenerator_.value();
        operatorParams->serviceTimeDistribution = serviceTimeDistribution.value();
        operatorParams->keysDistribution = keyDistribution.value();
        operatorParams->valuesDistribution = valueDistribution.value();
        operatorParams->operatorType = Operator::OperatorContinuousAggregation;

        // make the operator
        auto newOperator = OperatorBuilder::BuildOperator(operatorParams);

        // check if operator is valid
        assert(newOperator.has_value());





        // used to check the number of generated operations is right
        uint64_t  numberProducedOperations = 0;
        uint64_t  numberProducedGetOperations = 0;
        uint64_t  numberProducedPutOperations = 0;
        uint64_t  numberProducedDeleteOperations = 0;
        uint64_t  numberProducedMergeOperations = 0;

        // run the operator and check if it operates acts as expected
        while (newOperator.value()->HasNext() ) {
            numberProducedOperations ++;

            wrapper_.value()->operationsList.push_back(newOperator.value()->NextOperation());


            switch (static_cast<int>(wrapper_.value()->operationsList.back())) {
                case 0:
                    std::cout<< "get" <<std::endl;
                    numberProducedGetOperations ++;
                    break;
                case 1:
                    std::cout<< "put" <<std::endl;
                    numberProducedPutOperations ++;
                    break;
                case 2:
                    std::cout<< "merge" <<std::endl;
                    numberProducedMergeOperations ++;
                    break;
                case 3:
                    std::cout<< "delete" <<std::endl;
                    numberProducedDeleteOperations ++;
                    break;
            }




            wrapper_.value()->keysList.push_back(newOperator.value()->NextKey());
            //std::cout << "key: " << wrapper_.value()->keysList.back()<< std::endl; // print the window key
            wrapper_.value()->valuesList.push_back(newOperator.value()->NextValue());
            // std::cout << "value: " << wrapper_.value()->valuesList.back() << std::endl;
            wrapper_.value()->sleepTimes.push_back(newOperator.value()->NextSleepPeriod());
            // std::cout << "number of done operations: "<<numberProducedOperation <<std::endl;
        }


        // check if the keys are increasing- print them
        for (auto entry : wrapper_.value()->keysList) {
            std::cout << entry <<std::endl;
        }


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() == 15);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;


        // check  the operations  statistics
        std::cout<< numberProducedOperations << std::endl;
        assert(numberProducedPutOperations  == 80);
        assert(numberProducedOperations == 160);
        assert(numberProducedGetOperations  == 80  );
        assert(numberProducedDeleteOperations  == 0);
        assert(numberProducedMergeOperations  == 0);

        // check the value size
        assert (wrapper_.value()->valuesList.back().length() == 1024);

        // print the  last generated  value
        std::cout << "The last generated value is: " << wrapper_.value()->valuesList.back() << std::endl;


        // check the key size of the last generated key
        assert (wrapper_.value()->keysList.back().length() == 15);
        std::cout << "The last generated key is: " << wrapper_.value()->keysList.back() << std::endl;



        // check the last sleep time
        std::cout << "The last generated sleep  time: " << wrapper_.value()->sleepTimes.back() << std::endl;


        std::cout << "TestSlidingAllWindowIncremental: All tests are passed";

    }
#endif





       // TestS {}



   private:



    std::optional<std::shared_ptr<EventGenerator>> eventGenerator_;
    std::optional<std::shared_ptr<KVWrapper>>  wrapper_;



   };


   #endif //GADGET_TESTS_H
