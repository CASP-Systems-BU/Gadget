
# This cmake is for ubuntu  - please note that to evaluate a KV store with Gadget, you need  to install the KV store on your system and change this cmake
# we provide cmake for RocksDB - Faster and Berkeley kv stores, Please find them in cmakes folder

cmake_minimum_required(VERSION 3.16)
project(gadget)
set(BINARY ${CMAKE_PROJECT_NAME})




# use c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)




file(GLOB_RECURSE SOURCES src/*.cc)
file(GLOB_RECURSE INCLUDES src/include/*.h)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)
set(SOURCES ${SOURCES})

########################################################## setup rocksdb
message("--- locating rocksdb dependencies ---")
if(WIN32)
    find_package(RocksDB REQUIRED)
else()
    find_path(ROCKSDB_ROOT_DIR NAMES include/rocksdb/db.h)
    find_library(ROCKSDB_LIBRARIES NAMES rocksdb HINTS ${ROCKSDB_ROOT_DIR})
    find_path(ROCKSDB_INCLUDE_DIR NAMES rocksdb/db.h HINTS ${ROCKSDB_ROOT_DIR}/include)

    message("rocksdb   lib: ${ROCKSDB_LIBRARIES}")
    message("rocksdb   inc: ${ROCKSDB_INCLUDE_DIR}")
endif()





# include
include_directories("include")
add_executable(gadget core/main.cpp
        core/config.h
        include/gadget/distributions/key.h
        include/gadget/distributions/value.h
        include/gadget/distributions/arrival.h
        include/gadget/kvwrappers/kvwrapper.h
        include/gadget/kvwrappers/RocksDBWrapper.h
        include/gadget/kvwrappers/wrapperBuilder.h
        include/gadget/performanceMetrics.h
        core/PerformanceMetrics.cpp
        include/gadget/distributions/windowLength.h
        include/gadget/distributions/keySize.h
        include/gadget/distributions/keyPopularity.h
        operators/ycsbTrace.cpp
        include/gadget/operator/operators/continuousAggregation.h
        include/gadget/operator/operators/intervalJoin.h
        include/gadget/operator/operators/joinSliding.h
        include/gadget/operator/operators/intervalJoin.h
        include/gadget/operator/operators/joinTumbling.h
        include/gadget/operator/operators/operatorBuilder.h
        include/gadget/operator/operators/operatorParameters.h
        include/gadget/operator/operators/slidingAllHolistic.h
        include/gadget/operator/operators/slidingAllIncremental.h
        include/gadget/operator/operators/slidingKeyedIncremental.h
        include/gadget/operator/operators/slidingAllHolistic.h
        include/gadget/operator/operators/tumblingAllHolistic.h
        include/gadget/operator/operators/tumblingAllIncremental.h
        include/gadget/operator/operators/tumblingKeyedIncremental.h
        include/gadget/operator/operators/tumblingKeyedHolistic.h
        include/gadget/operator/operators/continuousJoin.h
        #include/gadget/operator/operators/sessionHolistic.h
        operators/flinkTrace.cpp
        include/gadget/distributions/serviceTime.h
        include/gadget/distributions/distributionParamters.h
        include/gadget/event/event.h
        include/gadget/event/eventGenerator.h
        operators/continuousJoin.cpp
        operators/slidingAllIncremental.cpp
        operators/slidingKeyedIncremental.cpp
        operators/joinTumbling.cpp
        operators/slidingKeyedHolistic.cpp
        operators/slidingAllHolistic.cpp
        operators/tumblingAllIncremental.cpp
        operators/tumblingAllIHolistic.cpp
        operators/tumblingKeyedIncremental.cpp
        operators/tumblingKeyedHolistic.cpp
        operators/joinSliding.cpp
        operators/continuousAggregation.cpp
        operators/intervalJoin.cpp
        #operators/sessionHolistic.cpp
        include/gadget/kvwrappers/fileWrapper.h
        core/tests.h
        include/gadget/event/eventGeneratorParameters.h
        include/gadget/statemachine/stateMachines.h
        include/gadget/operator/oneStreamContinuousOperator.h
        include/gadget/operator/oneStreamWindowOperator.h
        include/gadget/operator/oneStreamSessionWindowOperator.h
        include/gadget/operator/twoStreamContinuousOperator.h
        include/gadget/operator/twoStreamWindowOperator.h
        include/gadget/event/gadgetEventGenerator.h
        include/gadget/event/traceEvents.h
        include/gadget/statemachine/conJoinSM.h
        include/gadget/statemachine/intervalJoinSM.h
        include/gadget/statemachine/contAggrSM.h
        include/gadget/statemachine/windowHolisticSM.h
        include/gadget/statemachine/windowIncrementalSM.h
        include/gadget/kvwrappers/lethe.h
        include/gadget/kvwrappers/faster.h
        include/gadget/kvwrappers/berecklyDB.h
        include/gadget/operator/operators/ycsbReplayer.h
        include/gadget/operator/operators/flinkReplayer.h
        )
# make gadget  source a library to use it in google tests
add_library(${BINARY}_lib STATIC ${SOURCES})
message(${BINARY})

# make gadget  source a library to use it in google tests
add_library(${BINARY}_lib STATIC ${SOURCES})
message(${BINARY})

# turn on the followings for ubuntu
target_link_libraries(gadget pthread)
target_link_libraries(gadget dl)
target_link_libraries(gadget z)
target_link_libraries(gadget bz2)
target_link_libraries(gadget zstd)
target_link_libraries(gadget snappy)
#-lstdc++fs -luuid -ltbb -lgcc -laio -lm -lstdc++
target_link_libraries(gadget uuid)
target_link_libraries(gadget tbb )
target_link_libraries(gadget aio)
target_link_libraries(gadget m)
target_link_libraries(gadget stdc++fs)
target_link_libraries(gadget /usr/lib/x86_64-linux-gnu/liblz4.a)
target_link_libraries(gadget ssl)
target_link_libraries(gadget crypto)


