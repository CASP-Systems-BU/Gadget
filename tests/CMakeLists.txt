#set(BINARY ${CMAKE_PROJECT_NAME}_tst)

#file(GLOB_RECURSE SRC *.cc)



### setup rocksdb
#message("--- locating rocksdb dependencies ---")
#if(WIN32)
#    find_package(RocksDB REQUIRED)
#else()
#    find_path(ROCKSDB_ROOT_DIR NAMES include/rocksdb/db.h)
#    find_library(ROCKSDB_LIBRARIES NAMES rocksdb HINTS ${ROCKSDB_ROOT_DIR})
#    find_path(ROCKSDB_INCLUDE_DIR NAMES rocksdb/db.h HINTS ${ROCKSDB_ROOT_DIR}/include)

#    message("rocksdb   lib: ${ROCKSDB_LIBRARIES}")
#    message("rocksdb   inc: ${ROCKSDB_INCLUDE_DIR}")
#endif()

#include_directories(${CMAKE_SOURCE_DIR}/tests)

#add_executable(${BINARY} ${SRC} main.cpp)

#target_link_libraries(${BINARY}  gtest gtest_main)
#add_test(${BINARY} ${BINARY})

# 'Google_test' is the subproject name
cmake_minimum_required(VERSION 3.16)
project(Google_tests)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

include_directories(../src/include)

# 'Google_Tests_run' is the target name
# 'test1.cpp tests2.cpp' are source files with tests
add_executable(Google_Tests_run
        operators/ConAggrTest.cpp
        operators/tumblingAllIncremental.cpp
        operators/tumblingAllHolistic.cpp
        operators/tumblingKeyedHolistic.cpp
        operators/tumblingKeyedIncremental.cpp
        operators/slidingAllIncremental.cpp
        operators/slidingAllHolistic.cpp
        operators/slidingKeyedIncremental.cpp
        operators/slidingKeyedHolsitic.cpp
        operators/joinTumbling.cpp
        operators/joinSliding.cpp
        main.cpp)
target_link_libraries(Google_Tests_run PUBLIC   Gadget_lib gtest gtest_main)
add_test(NAME example_test COMMAND example)
