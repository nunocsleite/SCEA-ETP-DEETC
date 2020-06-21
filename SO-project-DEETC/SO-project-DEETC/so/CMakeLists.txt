
set(CMAKE_CXX_FLAGS "-fPIC")

# In order to use C++ 11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

cmake_minimum_required(VERSION 2.8)

# For Release compilation
set(CMAKE_BUILD_TYPE Release)

# OpenMP for paralelizing code
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")


# About this project
#-------------------------------
# SO-DEETC - Single Objective Optimisation for the ETP - Applied to the DEETC Benchmark Set
project(SO-DEETC)

#///////////////////////////////////////////////////////////////////////////
# CHANGED FROM ORIGINAL GITHUB PROJECT
include_directories(${SO-DEETC_SOURCE_DIR}/../../../ParadisEO-2.0/include/eo/)
include_directories(${SO-DEETC_SOURCE_DIR}/../../../ParadisEO-2.0/include/mo/)
include_directories(${SO-DEETC_SOURCE_DIR}/../../../ParadisEO-2.0/include/moeo/)

message(${SO-DEETC_SOURCE_DIR})

include_directories(${SO-DEETC_SOURCE_DIR}/../../../boost_1_73_0/)
include_directories(${SO-DEETC_SOURCE_DIR}/../../../armadillo/include/)
#///////////////////////////////////////////////////////////////////////////


include_directories(exes)


# Add subfolders
#-------------------------------
add_subdirectory(lib)
add_subdirectory(exes)
