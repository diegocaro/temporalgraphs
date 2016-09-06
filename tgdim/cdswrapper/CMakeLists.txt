cmake_minimum_required(VERSION 2.6.2)
project(cdswrapper CXX)

### Flags for all
SET(CMAKE_CXX_FLAGS "-std=c++11 -O3 -Wall -Wextra")

# Where .h files can be found.
include_directories(
  ${libcqtree_SOURCE_DIR}/include )

###
# libcds external
#
add_subdirectory(../../libcds/ ext_libcds)
include_directories(../../libcds/include)

###
# sdsl-lite external
##
add_subdirectory(../../sdsl-lite/ ext_sdsl-lite)
include_directories(../../sdsl-lite/include)

################################
# Unit Tests
################################
# enable_testing()
# Add test cpp file

add_library(cdswrapper src/dummy.cc)

target_include_directories(cdswrapper PUBLIC include)
target_link_libraries(cdswrapper cds sdsl)