cmake_minimum_required(VERSION 2.8)

set(THIS_NAME ctstring)

project(${THIS_NAME})

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

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX14)
if (COMPILER_SUPPORTS_CXX14)
    add_definitions(-std=c++14)
else ()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif ()

set(BUILD_FLAGS "-O3")
add_compile_options(
    -Werror 
    -Wall 
#    -Wextra 
    -Waddress
    -Warray-bounds
#    -Wconversion
    -Winit-self
#    -Wsuggest-attribute=const
    -Wunreachable-code
    -pedantic
    -pedantic-errors
)

add_executable(ut_${THIS_NAME} ${CXX_SRCS})

set_target_properties(ut_${THIS_NAME} PROPERTIES COMPILE_FLAGS 
    ${BUILD_FLAGS}
)


#set_source_files_properties(
#  ${CXX_SRCS} PROPERTIES COMPILE_FLAGS 
#  " -O3 -std=c++14 -Wall -Werror")


enable_testing()
add_test(UT ut_${THIS_NAME})
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS ut_${THIS_NAME})
