cmake_minimum_required(VERSION 2.8)
project(ctstring)

enable_language(CXX)


include_directories(
  ${PROJECT_SOURCE_DIR}
)

# Library path
set(CMAKE_LDFLAGS
  "${CMAKE_LDFLAGS} -L\".\" "
)

# Define the CXX sources
set ( CXX_SRCS
  string.cpp
  ut/ut_ctstring.cpp
)

set_source_files_properties(
  ${CXX_SRCS} PROPERTIES COMPILE_FLAGS 
  " -O3 -std=c++14 -Wall -Werror"
)

add_executable(ut_ctstring
  ${CXX_SRCS}
)

#include( CTest )
enable_testing()
add_test(UT ut_ctstring)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS ut_ctstring)
