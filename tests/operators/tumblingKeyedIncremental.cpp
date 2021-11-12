//
// Created by Showan Asyabi on 6/4/21.
//
//
// Created by Showan Asyabi on 6/4/21.
//

//
// Created by Showan Asyabi on 6/4/21.
//

//
// Created by Showan Asyabi on 5/26/21.
//

#include <iostream>
#include <memory>

#include "gtest/gtest.h"
#include "include/gadget/operation.h"


#include "include/gadget/distributions/distributionParamters.h"
#include "include/gadget/distributions/arrival.h"
#include "include/gadget/distributions/key.h"
#include "include/gadget/distributions/serviceTime.h"
#include "include/gadget/distributions/value.h"
#include "include/gadget/operator/operators/operatorParameters.h"
#include "include/gadget/operator/operators/operatorBuilder.h"
#include "include/gadget/event/eventGeneratorBuilder.h"
#include "include/gadget/event/eventGeneratorParameters.h"


class tumblingKeyedIncremental: public ::testing::Test {
public:
    tumblingKeyedIncremental( ) {
        eventGeneratorParams = std::make_shared<EventGeneratorParameters>();
        eventGeneratorParams->waterMarkFrequency = 1;
        auto e1 = std::make_shared<Event>();
        e1->eventTime_ = 0.1; // time is in second
        e1->key_ = "1";
        generatedEvents.push_back(e1);
        auto e2 = std::make_shared<Event>();
        e2->eventTime_ = 2;
        e2->key_ = "2";
        generatedEvents.push_back(e2);
        auto e3 = std::make_shared<Event>();
        e3->eventTime_ = 2.1;
        e3->key_ = "2";
        generatedEvents.push_back(e3);
        auto e4 = std::make_shared<Event>();
        e4->eventTime_ = 2.1;
        e4->key_ = "1";
        generatedEvents.push_back(e4);
        auto e5 = std::make_shared<Event>();
        e5->eventTime_ = 4.5;
        e5->key_ = "3";
        generatedEvents.push_back(e5);
        auto e6 = std::make_shared<Event>();
        e6->eventTime_ = 5;
        e6->key_ = "4";
        generatedEvents.push_back(e6);
        auto e7 = std::make_shared<Event>();
        e7->eventTime_ = 5;
        e7->key_ = "3";
        generatedEvents.push_back(e7);
        auto e8 = std::make_shared<Event>();
        e8->eventTime_ = 6.7;
        e8->key_ = "7";
        generatedEvents.push_back(e8);
        auto e9 = std::make_shared<Event>();
        e9->eventTime_ = 8.3;
        e9->key_ = "1";
        generatedEvents.push_back(e9);
        auto e11 = std::make_shared<Event>();
        e11->eventTime_ = 9;
        e11->key_ = "2";
        generatedEvents.push_back(e11);
        auto e10 = std::make_shared<Event>();
        e10->eventTime_ = 6.8; // out of order
        e10->key_ = "1";
        generatedEvents.push_back(e10);
        auto e12 = std::make_shared<Event>();
        e12->eventTime_ = 10;
        e12->key_ = "10";
        generatedEvents.push_back(e12);
    }

    void SetUp( ) {
        // make the event generator
        eventGeneratorParams->memEvents = generatedEvents;
        eventGen =  EventGeneratorBuilder::BuildEventGenerator(EventGenerator::MemEvents, eventGeneratorParams);
        // make the service time distributions - service time is a period of time needed by the streaming system to process a record
        auto serviceTimeDistributionParams = std::make_shared<DistributionParameters>();
        serviceTimeDistributionParams->lambda = 10;
        auto  serviceTimeDistribution = ServiceTimeBuilder::BuildServiceTime(ServiceTime::ExponentialDistrib, serviceTimeDistributionParams);


        // make the value size distribution- the  size of values  that are inserted to the state store (i.e., KV store)
        auto valueDistributionParams = std::make_shared<DistributionParameters>();
        valueDistributionParams->constantInt = 10;
        auto valueDistribution = ValueBuilder::BuildValue(Value::ConstantDistrib, valueDistributionParams);
        // make the key popularity distribution of windows keys
        auto keyPopularityDistributionParams = std::make_shared<DistributionParameters>();
        auto keySizeDistributionParams = std::make_shared<DistributionParameters>();
        keySizeDistributionParams -> constantInt = 10;
        auto keyPopDist =  KeyPopularityBuilder::BuildKeyPopularity(KeyPopularity::TemporalIncreasing,keyPopularityDistributionParams , 10000000);
        EXPECT_TRUE(keyPopDist.has_value());
        auto keySizeDist =  KeySizeBuilder::BuildKeySize(KeySize::ConstantDistrib, keySizeDistributionParams);
        EXPECT_TRUE(keySizeDist.has_value());
        auto keyDistribution = KeyBuilder::BuildKey(keyPopDist.value(),keySizeDist.value());

        // check if all of these distributions are made
        EXPECT_TRUE(serviceTimeDistribution.has_value());
        EXPECT_TRUE(valueDistribution.has_value());

        // make the operator

        operatorParams->expectedNumOperations = 100000;
        operatorParams->windowLength = 3;
        operatorParams->allowedLateness = false;
        operatorParams->latenessLength = 0;
        //operatorParams->slidingLength = 1;
        operatorParams->eventGenerator= eventGen.value();
        //operatorParams->eventGenerator2 = eventGen2.value();
        operatorParams->serviceTimeDistribution = serviceTimeDistribution.value();
        operatorParams->keysDistribution = keyDistribution;
        operatorParams->valuesDistribution = valueDistribution.value();
        // choose the operator type
        operatorParams->operatorType = Operator::OperatorWindowTumblingKeyedIncremental ;
        auto newOperator = OperatorBuilder::BuildOperator(operatorParams);
        //check if the operator has been made
        ASSERT_TRUE(newOperator.has_value());
        opr = newOperator.value();
    }

    void TearDown( ) {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be
    }

    ~tumblingKeyedIncremental( )  {
        // cleanup any pending stuff, but no exceptions allowed
    }
    std::shared_ptr<OperatorParameters>  operatorParams =  std::make_shared<OperatorParameters>();
    std::optional<std::shared_ptr<EventGenerator>>  eventGen;
    std::shared_ptr<EventGeneratorParameters> eventGeneratorParams;
    std::vector<std::shared_ptr<Event>> generatedEvents;
    std::shared_ptr<Operator> opr;
};


// this function checks if the number of generated access are correct
TEST_F(tumblingKeyedIncremental, tumblingKeyedIncrementalNumOpr) {
    std::vector<std::shared_ptr<Operation>> genOperations;
    // time unit 1
    // get first batch of state accesses
    genOperations.clear();
    opr->runOperator(genOperations);
    // for the first unit of time. There is  only one event; therefore, two operations are generated (a get and a put)
    EXPECT_EQ(genOperations.size(), 2);

    // time unit 2
    // get the second batch of events for unit time 2 (there is no event for this time unit)
    genOperations.clear();
    opr->runOperator(genOperations);
    ASSERT_EQ(genOperations.size(), 0);

    // time unit 3
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 10);

    // time unit 4
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 0);

    // time unit 5
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 2);

    // time unit 6
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 8);

    // time unit 7
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 2);

    // time unit  8
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 0);

    // time unit  9
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 6);

    // time unit 10
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 2);

    // time unit  11
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 2);

    // time unit 12
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 0);

}


// this function checks if the sequence of generated accesses are correct
// get ----> 0
// put ----> 1
// get --> 2
// delete -> 3
// scan ---> 4
// next ---> 5
TEST_F(tumblingKeyedIncremental, tumblingKeyedIncrementalOprSequence) {
    std::vector<std::shared_ptr<Operation>> genOperations;
    // time unit 1
    genOperations.clear();
    opr->runOperator(genOperations);
    ASSERT_EQ(genOperations.size(), 2);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); // get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put  operation
    EXPECT_EQ( genOperations[0]->key, "0000000001");
    EXPECT_EQ( genOperations[1]->key, "0000000001");

    // check the value size
    EXPECT_EQ( genOperations[0]->value.size(), 10);

    // get the second batch of events for unit time 2 (there is no event for this time unit)
    // time unit 2
    genOperations.clear();
    opr->runOperator(genOperations);
    ASSERT_EQ(genOperations.size(), 0);

    // unit time 3
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 10);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put operation
    EXPECT_EQ( static_cast<int>(genOperations[2]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[3]->oprType), 1); // put operation
    EXPECT_EQ( static_cast<int>(genOperations[4]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[5]->oprType), 1); // put operation
    EXPECT_EQ( static_cast<int>(genOperations[6]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[7]->oprType), 3); //  delete  operation
    EXPECT_EQ( static_cast<int>(genOperations[8]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[9]->oprType), 3); //  delete  operation

    // check the keys generated operations
    EXPECT_EQ( genOperations[0]->key, "0000000002");
    EXPECT_EQ( genOperations[1]->key, "0000000002");
    EXPECT_EQ( genOperations[2]->key, "0000000002");
    EXPECT_EQ( genOperations[3]->key, "0000000002");
    EXPECT_EQ( genOperations[4]->key, "0000000001");
    EXPECT_EQ( genOperations[5]->key, "0000000001");
    EXPECT_EQ( genOperations[6]->key, "0000000001");
    EXPECT_EQ( genOperations[7]->key, "0000000001");
    EXPECT_EQ( genOperations[8]->key, "0000000002");
    EXPECT_EQ( genOperations[9]->key, "0000000002");



    // unit time 4
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 0);

    // unit time 5
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 2);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); //  put  operation

    EXPECT_EQ( genOperations[0]->key, "0000000003");
    EXPECT_EQ( genOperations[1]->key, "0000000003");


    // unit time 6
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 8);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put operation
    EXPECT_EQ( static_cast<int>(genOperations[2]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[3]->oprType), 1); // put operation
    EXPECT_EQ( static_cast<int>(genOperations[4]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[5]->oprType), 3); //  delete  operation
    EXPECT_EQ( static_cast<int>(genOperations[6]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[7]->oprType), 3); //  delete  operation
    // check the keys generated operations
    EXPECT_EQ( genOperations[0]->key, "0000000004");
    EXPECT_EQ( genOperations[1]->key, "0000000004");
    EXPECT_EQ( genOperations[2]->key, "0000000003");
    EXPECT_EQ( genOperations[3]->key, "0000000003");
    EXPECT_EQ( genOperations[4]->key, "0000000003");
    EXPECT_EQ( genOperations[5]->key, "0000000003");
    EXPECT_EQ( genOperations[6]->key, "0000000004");
    EXPECT_EQ( genOperations[7]->key, "0000000004");

    // unit time 7
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 2);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put operation
    // check the keys generated operations
    EXPECT_EQ( genOperations[0]->key, "0000000005");
    EXPECT_EQ( genOperations[1]->key, "0000000005");

    // unit time 8
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 0);

    // unit time 9
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 6);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put operation
    EXPECT_EQ( static_cast<int>(genOperations[2]->oprType),  0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[3]->oprType), 3); //  delete  operation
    EXPECT_EQ( static_cast<int>(genOperations[4]->oprType),  0); //  delete   operation
    EXPECT_EQ( static_cast<int>(genOperations[5]->oprType), 3); //  delete  operation

    // check the keys generated operations
    EXPECT_EQ( genOperations[0]->key, "0000000006");
    EXPECT_EQ( genOperations[1]->key, "0000000006");
    EXPECT_EQ( genOperations[2]->key, "0000000005");
    EXPECT_EQ( genOperations[3]->key, "0000000005");
    EXPECT_EQ( genOperations[4]->key, "0000000006");
    EXPECT_EQ( genOperations[5]->key, "0000000006");


    // unit time 10
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 2);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put operation
    // check the keys generated operations
    EXPECT_EQ( genOperations[0]->key, "0000000007");
    EXPECT_EQ( genOperations[1]->key, "0000000007");


    // unit time 11
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 2);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put operation
    // check the keys generated operations
    EXPECT_EQ( genOperations[0]->key, "0000000008");
    EXPECT_EQ( genOperations[1]->key, "0000000008");


    // unit time 12
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 0);

}


// here we turn on lateness support to test this functionality
TEST_F(tumblingKeyedIncremental, latetumblingKeyedIncrementalOprSequence) {

    // change  the operator parameter to support lateness
    operatorParams->latenessLength = 2;
    operatorParams->allowedLateness = true;

    auto newOperator = OperatorBuilder::BuildOperator(operatorParams);
    //check if the operator has been made
    ASSERT_TRUE(newOperator.has_value());
    opr = newOperator.value();


    std::vector<std::shared_ptr<Operation>> genOperations;
    // time unit 1
    genOperations.clear();
    opr->runOperator(genOperations);
    ASSERT_EQ(genOperations.size(), 2);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); // get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put  operation
    EXPECT_EQ( genOperations[0]->key, "0000000001");
    EXPECT_EQ( genOperations[1]->key, "0000000001");

    // check the value size
    EXPECT_EQ( genOperations[0]->value.size(), 10);

    // get the second batch of events for unit time 2 (there is no event for this time unit)
    // time unit 2
    genOperations.clear();
    opr->runOperator(genOperations);
    ASSERT_EQ(genOperations.size(), 0);

    // unit time 3
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 6);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put operation
    EXPECT_EQ( static_cast<int>(genOperations[2]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[3]->oprType), 1); // put operation
    EXPECT_EQ( static_cast<int>(genOperations[4]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[5]->oprType), 1); // put operation


    // check the keys generated operations
    EXPECT_EQ( genOperations[0]->key, "0000000002");
    EXPECT_EQ( genOperations[1]->key, "0000000002");
    EXPECT_EQ( genOperations[2]->key, "0000000002");
    EXPECT_EQ( genOperations[3]->key, "0000000002");
    EXPECT_EQ( genOperations[4]->key, "0000000001");
    EXPECT_EQ( genOperations[5]->key, "0000000001");




    // unit time 4
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 0);

    // unit time 5
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 6);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); //  put  operation
    EXPECT_EQ( static_cast<int>(genOperations[2]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[3]->oprType), 3); //  delete  operation
    EXPECT_EQ( static_cast<int>(genOperations[4]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[5]->oprType), 3); //  delete  operation

    EXPECT_EQ( genOperations[0]->key, "0000000003");
    EXPECT_EQ( genOperations[1]->key, "0000000003");
    EXPECT_EQ( genOperations[2]->key, "0000000001");
    EXPECT_EQ( genOperations[3]->key, "0000000001");
    EXPECT_EQ( genOperations[4]->key, "0000000002");
    EXPECT_EQ( genOperations[5]->key, "0000000002");


    // unit time 6
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 4);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put operation
    EXPECT_EQ( static_cast<int>(genOperations[2]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[3]->oprType), 1); // put operation

    // check the keys generated operations
    EXPECT_EQ( genOperations[0]->key, "0000000004");
    EXPECT_EQ( genOperations[1]->key, "0000000004");
    EXPECT_EQ( genOperations[2]->key, "0000000003");
    EXPECT_EQ( genOperations[3]->key, "0000000003");


    // unit time 7
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 2);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put operation
    // check the keys generated operations
    EXPECT_EQ( genOperations[0]->key, "0000000005");
    EXPECT_EQ( genOperations[1]->key, "0000000005");

    // unit time 8
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 4);
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 3); //  delete  operation
    EXPECT_EQ( static_cast<int>(genOperations[2]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[3]->oprType), 3); //  delete  operation
    EXPECT_EQ( genOperations[0]->key, "0000000003");
    EXPECT_EQ( genOperations[1]->key, "0000000003");
    EXPECT_EQ( genOperations[2]->key, "0000000004");
    EXPECT_EQ( genOperations[3]->key, "0000000004");

    // unit time 9
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 2);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put operation


    // check the keys generated operations
    EXPECT_EQ( genOperations[0]->key, "0000000006");
    EXPECT_EQ( genOperations[1]->key, "0000000006");


    // unit time 10
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 4);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put operation
    EXPECT_EQ( static_cast<int>(genOperations[2]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[3]->oprType), 1); // put operation
    // check the keys generated operations
    EXPECT_EQ( genOperations[0]->key, "0000000007");
    EXPECT_EQ( genOperations[1]->key, "0000000007");
    EXPECT_EQ( genOperations[2]->key, "0000000006");
    EXPECT_EQ( genOperations[3]->key, "0000000006");


    // unit time 11
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 6);
    // check the sequence of generated operations
    EXPECT_EQ( static_cast<int>(genOperations[0]->oprType), 0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[1]->oprType), 1); // put operation
    EXPECT_EQ( static_cast<int>(genOperations[2]->oprType),  0); //  get  operation
    EXPECT_EQ( static_cast<int>(genOperations[3]->oprType), 3); //  delete  operation
    EXPECT_EQ( static_cast<int>(genOperations[4]->oprType),  0); //  delete   operation
    EXPECT_EQ( static_cast<int>(genOperations[5]->oprType), 3); //  delete  operation
    // check the keys generated operations
    EXPECT_EQ( genOperations[0]->key, "0000000008");



    EXPECT_EQ( genOperations[1]->key, "0000000008");
    EXPECT_EQ( genOperations[2]->key, "0000000005");
    EXPECT_EQ( genOperations[3]->key, "0000000005");
    EXPECT_EQ( genOperations[4]->key, "0000000006");
    EXPECT_EQ( genOperations[5]->key, "0000000006");



    // unit time 12
    genOperations.clear();
    opr->runOperator(genOperations);
    EXPECT_EQ(genOperations.size(), 0);

}






