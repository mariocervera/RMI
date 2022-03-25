cmake_minimum_required(VERSION 3.18)

project(RMI)

# Load FetchContent module.
include(FetchContent)

# Declare GoogleTest as the content to fetch
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.10.0
)

# Fetch GoogleTest and make targets available
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

set(TEST_NAME MethodCallerTest)

add_executable(${TEST_NAME})

target_sources(${TEST_NAME} PRIVATE MethodCallerTest.cpp)

target_link_libraries(${TEST_NAME} gtest gtest_main)

target_compile_features(${TEST_NAME} PRIVATE cxx_std_17)
