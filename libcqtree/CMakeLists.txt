cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AppendCompilerFlags)

project(libcqtree CXX)

### Flags for all
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

# Where .h files can be found.
include_directories(
  ${libcqtree_SOURCE_DIR}/include
  ${libcqtree_SOURCE_DIR}/src)

# Where libraries can be found.
# link_directories(${gtest_BINARY_DIR}/src)

################################
# GTest
################################
add_subdirectory (../external/gtest-1.7.0 ext_gtest-1.7.0)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

###
# cdswrapper, sdsl and libcds are automatically included :D
##
add_subdirectory(../cdswrapper ext_cdswrapper)
#include_directories(../cdswrapper/include)

#libcds
#add_subdirectory(../../libcds ext_libcds)
#include_directories(../../libcds/include)

#sdsl
#add_subdirectory(../../sdsl-lite ext_sdsl-lite)
#include_directories(../../sdsl-lite/include)

#library for compressing integers
#add_subdirectory(../../compresslists ext_compresslists)

################################
# Unit Tests
################################
enable_testing()
# Add test cpp file

add_executable( runPointUnitTests test/point_test.cc )
# Link test executable against gtest & gtest_main
target_link_libraries(runPointUnitTests gtest gtest_main)
add_test( runPointUnitTests runPointUnitTests )

add_executable( runCodeUnitTests test/code_test.cc )
# Link test executable against gtest & gtest_main
target_link_libraries(runCodeUnitTests gtest gtest_main cqtree)
add_test( runCodeUnitTests runCodeUnitTests )

add_executable( runMXUnitTests test/mx_test.cc  )
target_link_libraries(runMXUnitTests gtest gtest_main cqtree)
add_test( runMXUnitTests runMXUnitTests )

add_executable( runPRBUnitTests test/prb_test.cc  )
target_link_libraries(runPRBUnitTests gtest gtest_main cqtree)
add_test( runPRBUnitTests runPRBUnitTests )

add_executable( runPRB2UnitTests test/prb2_test.cc  )
target_link_libraries(runPRB2UnitTests gtest gtest_main cqtree)
add_test( runPRB2UnitTests runPRB2UnitTests )

### Common objects files in my project
add_library(cqtree src/utils.cc src/CompactQtree.cc src/MXCompactQtree.cc src/PRBCompactQtree.cc src/PRB2CompactQtree.cc src/bitvector.cc )

target_include_directories(cqtree PUBLIC src)
target_include_directories(cqtree PUBLIC include)

target_link_libraries(cqtree cdswrapper)


configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Make.helper.cmake"
		       "${CMAKE_CURRENT_SOURCE_DIR}/Make.helper" @ONLY)
