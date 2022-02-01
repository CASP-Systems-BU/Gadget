
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
# if you have installed rocksDB on your system, uncomment the following block
#[[
if(WIN32)
    target_link_libraries(gadget RocksDB::rocksdb)
else()
    include_directories(SYSTEM ${ROCKSDB_INCLUDE_DIR})
    target_link_libraries(gadget ${ROCKSDB_LIBRARIES})
    # turn on the followings for ubuntu
    #target_link_libraries(gadget pthread)
    #target_link_libraries(gadget dl)
    #target_link_libraries(gadget z)
    #target_link_libraries(gadget bz2)
    #target_link_libraries(gadget zstd)
    #target_link_libraries(gadget snappy)
    #target_link_libraries(gadget /usr/lib/x86_64-linux-gnu/liblz4.a)
endif()
]]
################### end of rocksDB part

###### for experiments use the following cmake
#[[
# to  use lethe : cmake .. -DUSE_LETHE=ON
cmake_minimum_required(VERSION 3.16)
project(gadget)
set(BINARY ${CMAKE_PROJECT_NAME})

#for merge operator in rocksdb
#target_compile_options(foo PUBLIC -fno-rtti)


# use c++ 14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#find_package(Gflags REQUIRED)
#find_package(Glog REQUIRED)



file(GLOB_RECURSE SOURCES src/*.cc)
file(GLOB_RECURSE INCLUDES src/include/*.h)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)
set(SOURCES ${SOURCES} include/gadget/kvwrappers/trash.h)
### setup faster
find_library(FASTERLIB faster /home/king/Desktop/wassGitHub/wassm/dbs/faster/FASTER/cc/build)
#find_path(FASTER_INCLUDE_DIR  NAME /home/king/Desktop/wassGitHub/wassm/dbs/faster/FASTER/cc/src)
set(FASTERDIR /home/king/Desktop/wassGitHub/wassm/dbs/faster/FASTER/cc/src)
message("faster   lib: ${FASTERLIB}")




###################### setup berkly db
find_library(BERCLYLIB db_cxx  /home/king/Downloads/berkerlydb)
#find_path(FASTER_INCLUDE_DIR  NAME /home/king/Desktop/wassGitHub/wassm/dbs/faster/FASTER/cc/src)
set(BERCLYDIR /home/king/Downloads/berkerlydb)
message("berkly   lib: ${BERCLYLIB}")

###########################


option(USE_LETHE "Use LETHE instead of ROCKSDB" OFF) #OFF by default

#find_path(LETHE_ROOT_DIR NAMES /home/king/Downloads/lethe)
#find_library(LETHE_LIBRARIES NAMES rocksdb HINTS ${LETHE_ROOT_DIR})
#find_path(LETHE_INCLUDE_DIR NAMES rocksdb/db.h HINTS ${LETHE_ROOT_DIR}/include)
#find_library(LETHE_LIBRARIES  rocksdb /home/king/Downloads/lethe)
#find_path(FASTER_INCLUDE_DIR  NAME /home/king/Desktop/wassGitHub/wassm/dbs/faster/FASTER/cc/src)
set(LETHE_LIBRARIES /home/king/Downloads/lethe/librocksdb.a)
set(LETHE_INCLUDE_DIR /home/king/Downloads/lethe/include)
set(LETHE_WORKING_BRANCH /home/king/Downloads/lethe/examples/__working_branch)


message("lethe   lib: ${LETHE_LIBRARIES}")
message("lethe   inc: ${LETHE_INCLUDE_DIR}")


### setup rocksdb
message("--- locating rocksdb dependencies ---")
if(WIN32)
    find_package(RocksDB REQUIRED)
else()
    find_path(ROCKSDB_ROOT_DIR NAMES include/rocksdb/db.h)
    find_library(ROCKSDB_LIBRARIES NAMES rocksdb HINTS ${ROCKSDB_ROOT_DIR})
    find_path(ROCKSDB_INCLUDE_DIR NAMES rocksdb/db.h HINTS ${ROCKSDB_ROOT_DIR}/include)
endif()






# include
include_directories("include")
#include_directories("/usr/local/Cellar/rocksdb/6.15.4/include")

#######################
#ADD_LIBRARY(myRocks  STATIC IMPORTED)
#SET_TARGET_PROPERTIES(myRocks PROPERTIES
#        IMPORTED_LOCATION /usr/local/Cellar/rocksdb/6.15.4/lib/librocksdb.a)


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
        include/gadget/kvwrappers/fileWrapper.h
        core/tests.h
        include/gadget/event/eventGeneratorParameters.h
        include/gadget/statemachine/stateMachines.h
        include/gadget/operator/oneStreamContinuousOperator.h
        include/gadget/operator/oneStreamWindowOperator.h
        include/gadget/operator/twoStreamContinuousOperator.h
        include/gadget/operator/twoStreamWindowOperator.h
        include/gadget/event/gadgetEventGenerator.h
        include/gadget/event/traceEvents.h
        include/gadget/statemachine/conJoinSM.h
        include/gadget/statemachine/contAggrSM.h
        include/gadget/statemachine/windowHolisticSM.h
        include/gadget/statemachine/intervalJoinSM.h
        include/gadget/statemachine/windowIncrementalSM.h
        include/gadget/kvwrappers/lethe.h
        include/gadget/kvwrappers/faster.h
        include/gadget/kvwrappers/berecklyDB.h
        #  change the following paths  based on the location of the LetheDB
        /home/king/Downloads/lethe/examples/__working_branch/stats.cc
        /home/king/Downloads/lethe/examples/__working_branch/emu_environment.cc
        )
add_library(${BINARY}_lib STATIC ${SOURCES})
#if(WIN32)
#    target_link_libraries(gadget RocksDB::rocksdb)
#else()

if(USE_LETHE)
    include_directories(gadget   ${LETHE_INCLUDE_DIR})
    target_link_libraries(gadget ${LETHE_LIBRARIES})
    include_directories(gadget ${LETHE_WORKING_BRANCH})
    message("lethe   lib: ${LETHE_LIBRARIES}")
    message("lethe   inc: ${LETHE_INCLUDE_DIR}")
    message("lethe   inc2: ${LETHE_WORKING_BRANCH}")
else() # use rocks db
    include_directories(SYSTEM ${ROCKSDB_INCLUDE_DIR})
    target_link_libraries(gadget ${ROCKSDB_LIBRARIES})
    message("rocksdb   lib: ${ROCKSDB_LIBRARIES}")
    message("rocksdb   inc: ${ROCKSDB_INCLUDE_DIR}")
endif(USE_LETHE)
include_directories(gadget   ${FASTERDIR})
target_link_libraries(gadget ${FASTERLIB})
include_directories(gadget   ${BERCLYDIR})
target_link_libraries(gadget ${BERCLYLIB})
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
#endif()

unset(USE_LETHE CACHE) # <---- this is the important!!
unset(ROCKSDB_LIBRARIES CACHE) # <---- this is the important!!
unset(ROCKSDB_INCLUDE_DIR CACHE) # <---- this is the important!!

]]
