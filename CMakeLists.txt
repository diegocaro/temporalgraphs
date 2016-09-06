########################################################################
# CMake build script for libcds.


########################################################################
#
# Project-wide settings

# Name of the project.
#
# CMake files in this project can refer to the root source directory
# as ${cds_SOURCE_DIR} and to the root binary directory as
# ${cds_BINARY_DIR}.


cmake_minimum_required(VERSION 2.8.7)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AppendCompilerFlags)

project(cds CXX)

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







# Where libcds .h files can be found.
include_directories(
  ${cds_SOURCE_DIR}/include
  ${cds_SOURCE_DIR})

add_library(cds 
src/static/bitsequence/BitSequence.cpp
src/static/bitsequence/BitSequenceBuilderDArray.cpp
src/static/bitsequence/BitSequenceBuilderRG.cpp
src/static/bitsequence/BitSequenceBuilderRRR.cpp
src/static/bitsequence/BitSequenceBuilderSDArray.cpp
src/static/bitsequence/BitSequenceDArray.cpp
src/static/bitsequence/BitSequenceRG.cpp
src/static/bitsequence/BitSequenceRRR.cpp
src/static/bitsequence/BitSequenceSDArray.cpp
src/static/bitsequence/sdarraySadakane.cpp
src/static/bitsequence/TableOffsetRRR.cpp
src/static/coders/huff.cpp
src/static/coders/HuffmanCoder.cpp
src/static/mapper/Mapper.cpp
src/static/mapper/MapperCont.cpp
src/static/mapper/MapperNone.cpp
src/static/mapper/MapperRev.cpp
src/static/permutation/perm.cpp
src/static/permutation/Permutation.cpp
src/static/permutation/PermutationBuilderMRRR.cpp
src/static/permutation/PermutationBuilderWT.cpp
src/static/permutation/PermutationMRRR.cpp
src/static/permutation/PermutationWT.cpp
src/static/sequence/BitmapsSequence.cpp
src/static/sequence/Sequence.cpp
src/static/sequence/SequenceAlphPart.cpp
src/static/sequence/SequenceBuilderAlphPart.cpp
src/static/sequence/SequenceBuilderGMR.cpp
src/static/sequence/SequenceBuilderGMRChunk.cpp
src/static/sequence/SequenceBuilderStr.cpp
src/static/sequence/SequenceBuilderWaveletMatrix.cpp
src/static/sequence/SequenceBuilderWaveletTree.cpp
src/static/sequence/SequenceBuilderWaveletTreeNoptrs.cpp
src/static/sequence/SequenceBuilderWaveletTreeNoptrsS.cpp
src/static/sequence/SequenceGMR.cpp
src/static/sequence/SequenceGMRChunk.cpp
src/static/sequence/WaveletMatrix.cpp
src/static/sequence/WaveletTree.cpp
src/static/sequence/WaveletTreeNoptrs.cpp
src/static/sequence/WaveletTreeNoptrsS.cpp
src/static/sequence/wt_coder.cpp
src/static/sequence/wt_coder_binary.cpp
src/static/sequence/wt_coder_huff.cpp
src/static/sequence/wt_node.cpp
src/static/sequence/wt_node_internal.cpp
src/static/sequence/wt_node_leaf.cpp
src/utils/Array.cpp
src/utils/cppUtils.cpp
src/utils/libcdsBitString.cpp
src/utils/timing.cpp
)
target_include_directories(cds PUBLIC include)

