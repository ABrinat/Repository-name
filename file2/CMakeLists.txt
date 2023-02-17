
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(minbias)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{ROOT_USE_FILE}")

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
#list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOT_USE_FILE})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree Net)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
add_definitions(${ROOT_CXX_FLAGS})

ROOT_GENERATE_DICTIONARY(G__MinBias  MinBiasTree.h SClass.h LINKDEF LinkDef.h)

#---Create a shared library with geneated dictionary
add_library(MinBiasLib SHARED  MinBiasTree.cxx SClass.cxx G__MinBias.cxx)
target_link_libraries(MinBiasLib ${ROOT_LIBRARIES})


# ---Build executable------------------------------------------------------------

add_executable(minbias minbias.cxx)
target_link_libraries(minbias MinBiasLib)

