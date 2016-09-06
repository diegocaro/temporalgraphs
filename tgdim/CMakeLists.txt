# cmake -DCMAKE_BUILD_TYPE=Release .  //for experiments
# cmake -DCMAKE_BUILD_TYPE=Debug . //for debugging

cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(AppendCompilerFlags)
include(RemoveFlags)

project(tgcq CXX)


option(CODE_COVERAGE "Set ON to add code coverage compile options" OFF)

# C++11 compiler Check
if(NOT CMAKE_CXX_COMPILER_VERSION) # work around for cmake versions smaller than 2.8.10
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)
endif()
if(CMAKE_CXX_COMPILER MATCHES ".*clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()
if( (CMAKE_COMPILER_IS_GNUCXX AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.7) OR
    (CMAKE_COMPILER_IS_CLANGXX AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.2))
  message(FATAL_ERROR "Your C++ compiler does not support C++11. Please install g++ 4.7 (or greater) or clang 3.2 (or greater)")
else()
  message(STATUS "Compiler is recent enough to support C++11.")
endif()

if( CMAKE_COMPILER_IS_GNUCXX )
    #append_cxx_compiler_flags("-std=c++11 -Wall -Wextra  -DNDEBUG" "GCC" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-std=c++11 -Wall -Wextra" "GCC" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-O3 -ffast-math -funroll-loops" "GCC" CMAKE_CXX_OPT_FLAGS)
    if ( CODE_COVERAGE )
        append_cxx_compiler_flags("-g -fprofile-arcs -ftest-coverage -lgcov" "GCC" CMAKE_CXX_FLAGS)
    endif()
else()
    #append_cxx_compiler_flags("-std=c++11 -DNDEBUG" "CLANG" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-std=c++11 -Wall -Wextra" "CLANG" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-stdlib=libc++" "CLANG" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-O3 -ffast-math -funroll-loops" "CLANG" CMAKE_CXX_OPT_FLAGS)
endif()

remove_cxx_flag("-DNDEBUG")
message(STATUS "Flags:" ${CMAKE_CXX_FLAGS_RELEASE})



# Where .h files can be found.
#include_directories(
#  ${CMAKE_CURRENT_SOURCE_DIR}/include
#  ${CMAKE_CURRENT_SOURCE_DIR}/src)

# libcqtree 
# this include sdsl cds cdswrapper
add_subdirectory(libcqtree)

###
# cdswrapper
##
#add_subdirectory(cdswrapper)
#include_directories(cdswrapper/include)

##
#libcds
##
#add_subdirectory(../libcds ext_libcds)
#include_directories(../libcds/include)

##
#sdsl
##
#add_subdirectory(../sdsl-lite ext_sdsl-lite)
#include_directories(../sdsl-lite/include)


### Default parameter for debug
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2 -Wall -Wextra")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   message(STATUS "Makefile for Debug mode")
   
   message(STATUS "Counting rank/select/access operations")
   set_property (DIRECTORY libcqtree PROPERTY COMPILE_DEFINITIONS "COUNTOPS")
   add_definitions( -DCOUNTOPS=1 )
endif()

### Default parametrs for release
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ")
#if (CMAKE_BUILD_TYPE STREQUAL "Release")
#   message(STATUS "Makefile for Release mode")
#endif()

add_library( tempgraph arraysort.cc TemporalGraph.cc)
target_link_libraries( tempgraph cqtree)

add_executable( update update.cc )
target_link_libraries(update tempgraph)


add_executable( create create.cc)
target_link_libraries(create tempgraph)

add_executable( sort sort.cc )
target_link_libraries(sort tempgraph)


add_executable( benchmark benchmark.cc)
target_link_libraries(benchmark tempgraph)
set_target_properties (benchmark PROPERTIES COMPILE_DEFINITIONS "EXPERIMENTS")

add_executable( benchmark_time benchmark.cc)
target_link_libraries(benchmark_time tempgraph)
set_target_properties (benchmark_time PROPERTIES COMPILE_DEFINITIONS TIMESAMPLE)
set_property(TARGET benchmark_time APPEND PROPERTY COMPILE_DEFINITIONS EXPERIMENTS)


add_executable( checkans benchmark.cc )
target_link_libraries(checkans tempgraph)

add_executable( stats stats.cc )
target_link_libraries(stats tempgraph)

add_executable( leaves leaves.cc )
target_link_libraries(leaves tempgraph)

add_executable( allvertices allvertices.cc )
target_link_libraries(allvertices tempgraph)

add_executable( allvertices_time allvertices.cc )
target_link_libraries(allvertices_time tempgraph)
set_target_properties (allvertices_time PROPERTIES COMPILE_DEFINITIONS TIMESAMPLE)


add_executable( exp-query exp-query.cc )
target_link_libraries(exp-query tempgraph)


add_executable( vertices vertices.cc )
target_link_libraries(vertices tempgraph)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(benchmark -lrt)
    target_link_libraries(checkans -lrt)
    target_link_libraries(benchmark_time -lrt)
	target_link_libraries(allvertices -lrt)
	target_link_libraries(allvertices_time -lrt)
    target_link_libraries(exp-query -lrt)
    target_link_libraries(vertices -lrt)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
