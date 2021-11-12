cmake_minimum_required(VERSION 3.16)
project(Gadget)




# use c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)



include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)