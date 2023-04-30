cmake_minimum_required(VERSION 3.10.0)
project(chickens-and-more)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system regex log_setup log)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# find the bash program
find_program (BASH_PROGRAM bash)

# TODO(!): Update name and srcs
add_library(server_lib src/server.cc)
add_library(session_lib src/session.cc)
add_library(reply_lib src/reply.cc)
add_library(config_parser_lib src/config_parser.cc)

# Update executable name, srcs, and deps
add_executable(webserver src/server_main.cc)
# target_link_libraries
# add_executable(webserver src/server_main.cc src/server.cc src/session.cc src/reply.cc)
target_link_libraries(webserver server_lib session_lib reply_lib config_parser_lib Boost::log_setup Boost::log Boost::system Boost::regex pthread)


# Update test executable name, srcs, and deps
add_executable(config_parser_test tests/config_parser_test.cc)
add_executable(reply_test tests/reply_test.cc)
add_executable(session_test tests/session_test.cc)
target_link_libraries(config_parser_test config_parser_lib Boost::log_setup Boost::log gtest_main gmock_main)
target_link_libraries(reply_test reply_lib gtest_main gmock_main)
target_link_libraries(session_test session_lib reply_lib Boost::system Boost::regex Boost::log_setup Boost::log gtest_main gmock_main)

# Update with test binary
# gtest_discover_tests(example_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(reply_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

#Add integration Test
if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Coverage")
add_test(NAME integration_tests COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()

# TODO(!): Update with target/test targets
# include(cmake/CodeCoverageReportConfig.cmake)
# generate_coverage_report(TARGETS example_server example_lib TESTS example_lib_test)
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS config_parser_lib reply_lib session_lib TESTS config_parser_test reply_test session_test)