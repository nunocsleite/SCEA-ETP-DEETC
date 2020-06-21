# project
#-------------------------------
project(exes)

set(CMAKE_CXX_FLAGS "-fPIC")

# Set public header list (add your headers and source files here))
#-------------------------------
set(${PROJECT_NAME}_headers )
set(${PROJECT_NAME}_sources TestSO.cpp)

# Create binaries
#-------------------------------
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_headers} ${${PROJECT_NAME}_sources})

# Link
#-------------------------------
#target_link_libraries(${PROJECT_NAME} eo es moeo cma eoutils ga armadillo)

target_link_libraries(${PROJECT_NAME} SOlib)



find_library(LIBeo eo ${PROJECT_SOURCE_DIR})
find_library(LIBes es ${PROJECT_SOURCE_DIR})
find_library(LIBmoeo moeo ${PROJECT_SOURCE_DIR})
find_library(LIBcma cma ${PROJECT_SOURCE_DIR})
find_library(LIBeoutils eoutils ${PROJECT_SOURCE_DIR})
find_library(LIBga ga ${PROJECT_SOURCE_DIR})

message(${LIBeo})
message(${LIBes})
message(${LIBmoeo})
message(${LIBcma})
message(${LIBeoutils})
message(${LIBga})

#target_link_libraries(${PROJECT_NAME} ${LIBeo} ${LIBes} ${LIBmoeo} ${LIBcma} ${LIBeoutils} ${LIBga})
target_link_libraries(${PROJECT_NAME} ${LIBeo} ${LIBes} ${LIBcma} ${LIBeoutils} ${LIBga})
