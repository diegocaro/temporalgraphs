cmake_minimum_required(VERSION 2.8.7)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AppendCompilerFlags)
 
project(cas)
 
### Flags for all
SET(CMAKE_CXX_FLAGS "-Wall -DEXPERIMENTS")
 
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
    append_cxx_compiler_flags("-std=c++11 -Wall -Wextra  -DNDEBUG" "GCC" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-O3 -ffast-math -funroll-loops" "GCC" CMAKE_CXX_OPT_FLAGS)
    if ( CODE_COVERAGE )
        append_cxx_compiler_flags("-g -fprofile-arcs -ftest-coverage -lgcov" "GCC" CMAKE_CXX_FLAGS)
    endif()
else()
    append_cxx_compiler_flags("-std=c++11 -DNDEBUG" "CLANG" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-stdlib=libc++" "CLANG" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-O3 -ffast-math -funroll-loops" "CLANG" CMAKE_CXX_OPT_FLAGS)
endif()
 
 
### Default parameter for debug
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -DDEBUG")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   message(STATUS "Makefile for Debug mode")
endif()
 
### Default parametrs for release
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
 
### wmatrix+libcds static
add_subdirectory(../wmatrix/ ext_wmatrix)
 
### Common objects files
 
#add_library(Objs OBJECT tgs.cpp mywtmatrix.cpp arraysort.cpp )
add_library(cas tgs.cpp mywtmatrix.cpp arraysort.cpp )
target_link_libraries(cas wmatrix)

add_executable(create create.cpp )
target_link_libraries(create cas)
  
add_executable(benchmark benchmark.cpp )
target_link_libraries(benchmark cas)
 
add_executable(benchmark2 benchmark2.cpp )
target_link_libraries(benchmark2 cas)
 
add_executable(exp-query exp-query.cpp )
target_link_libraries(exp-query cas)

add_executable(vertices vertices.cpp )
target_link_libraries(vertices cas)
 
#install(TARGETS create DESTINATION bin)
 
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries(benchmark -lrt)
target_link_libraries(benchmark2 -lrt)
target_link_libraries(exp-query -lrt)
target_link_libraries(vertices -lrt)
endif ()