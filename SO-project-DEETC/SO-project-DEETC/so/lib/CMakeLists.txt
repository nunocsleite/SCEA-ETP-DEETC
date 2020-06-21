# project
#-------------------------------
project(SOlib)

# Set public header list (add your headers and source files here))
#-------------------------------
set(${PROJECT_NAME}_headers
                        Chromosome.h
                        Crossover.h
                        Common.h
                        Data.h
                        DateTime.h
                        DEETCTestSet.h
                        eoChromosome.h
                        eoETTPEval.h
                        eoETTPNumberClashesEval.h
                        eoEvolutionOperator.h
                        eoExtendedChromosome.h
                        eoGenerationContinue.h
                        eoSCEA.h
                        eoSimpleGA.h
                        ETTPInit.h
                        ETTPKempeChainHeuristic.h
                        ETTPneighbor.h
                        ETTPneighborEval.h
                        ETTPneighborhood.h
                        Exam.h
                        ExtendedChromosome.h
                        GraphColouringHeuristics.h
                        Matrix.h
                        moGDA.h
                        moGDAexplorer.h
                        moSA.h
                        moSAexplorer.h
                        moSimpleCoolingSchedule.h
                        moTA.h
                        moTAexplorer.h
                        Mutation.h
                        Period.h
                        ProximityCostEval.h
                        Room.h
                        TestSet.h
                        TestSetDescription.h
                        TorontoTestSet.h
                        VertexPriorityQueue.h
)

set(${PROJECT_NAME}_sources
                        Chromosome.cpp
                        Crossover.cpp
                        DateTime.cpp
                        DEETCTestSet.cpp
                        eoChromosome.cpp
                        eoExtendedChromosome.cpp
                        ExtendedChromosome.cpp
                        GraphColouringHeuristics.cpp
                        MainApp.cpp
                        Matrix.cpp
                        Mutation.cpp
                        TestSetDescription.cpp
                        TorontoTestSet.cpp
                        Utils.cpp
                        VertexPriorityQueue.cpp
)


add_library(SOlib SHARED ${${PROJECT_NAME}_headers} ${${PROJECT_NAME}_sources})


find_library(LIBeo eo ${PROJECT_SOURCE_DIR})
find_library(LIBes es ${PROJECT_SOURCE_DIR})
find_library(LIBmoeo moeo ${PROJECT_SOURCE_DIR})
find_library(LIBcma cma ${PROJECT_SOURCE_DIR})
find_library(LIBeoutils eoutils ${PROJECT_SOURCE_DIR})
find_library(LIBga ga ${PROJECT_SOURCE_DIR})

#message(${LIBeo})
#message(${PROJECT_NAME})

#message(PROJECT SOURCE DIR:)
#message(${PROJECT_SOURCE_DIR})

#target_link_libraries(${PROJECT_NAME} ${LIBeo} ${LIBes} ${LIBmoeo} ${LIBcma} ${LIBeoutils} ${LIBga})
target_link_libraries(${PROJECT_NAME} ${LIBeo} ${LIBes} ${LIBcma} ${LIBeoutils} ${LIBga})

# Include ParadisEO, Boost Regex, and Armadillo libs
#target_link_libraries(${PROJECT_NAME} boost_regex eo es moeo cma eoutils ga armadillo)
#target_link_libraries(${PROJECT_NAME} eo es moeo cma eoutils ga)





