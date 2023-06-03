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

add_library(server_lib src/server.cc)
add_library(session_lib src/session.cc src/echo_handler.cc src/static_handler.cc src/not_found_handler.cc src/error_handler.cc src/sleep_handler.cc src/health_handler.cc src/chess_handler.cc)
add_library(echo_handler_lib src/echo_handler.cc)
add_library(not_found_handler_lib src/not_found_handler.cc)
add_library(error_handler_lib src/error_handler.cc)
add_library(sleep_handler_lib src/sleep_handler.cc)
add_library(static_handler_lib src/static_handler.cc)
add_library(health_handler_lib src/health_handler.cc)
add_library(chess_handler_lib src/chess_handler.cc)
add_library(reply_lib src/echo_handler.cc src/reply.cc)
add_library(config_parser_lib src/config_parser.cc)
add_library(crud_handler_lib src/crud_handler.cc)
add_library(filesystem_lib src/persistent_filesystem.cc)
add_library(mock_filesystem_lib src/mock_filesystem.cc)


add_library(echo_handler_factory_lib src/echo_handler_factory.cc src/echo_handler.cc)
add_library(static_handler_factory_lib src/static_handler_factory.cc src/static_handler.cc)
add_library(crud_handler_factory_lib src/crud_handler_factory.cc src/crud_handler.cc)
add_library(sleep_handler_factory_lib src/sleep_handler_factory.cc src/sleep_handler.cc)
add_library(health_handler_factory_lib src/health_handler_factory.cc src/health_handler.cc)
add_library(chess_handler_factory_lib src/chess_handler_factory.cc src/chess_handler.cc)

add_library(chess_lib src/chess/board.cpp src/chess/defs.cpp src/chess/movegen.cpp src/chess/searcher.cpp)

# Executable name, srcs, and deps
add_executable(webserver src/server_main.cc)
target_link_libraries(webserver server_lib session_lib not_found_handler_lib error_handler_lib echo_handler_lib sleep_handler_lib chess_handler_lib health_handler_lib static_handler_lib crud_handler_lib reply_lib config_parser_lib echo_handler_factory_lib static_handler_factory_lib chess_handler_factory_lib health_handler_factory_lib crud_handler_factory_lib sleep_handler_factory_lib filesystem_lib chess_lib Boost::log_setup Boost::log Boost::system Boost::regex pthread)


# Test executable name, srcs, and deps
add_executable(config_parser_test tests/config_parser_test.cc)
add_executable(reply_test tests/reply_test.cc)
add_executable(session_test tests/session_test.cc)
add_executable(static_handler_test tests/static_handler_test.cc)
add_executable(echo_handler_test tests/echo_handler_test.cc)
add_executable(crud_handler_test tests/crud_handler_test.cc)
add_executable(sleep_handler_test tests/sleep_handler_test.cc)
add_executable(health_handler_test tests/health_handler_test.cc)
add_executable(not_found_handler_test tests/not_found_handler_test.cc)
add_executable(error_handler_test tests/error_handler_test.cc)
add_executable(sleep_handler_factory_test tests/sleep_handler_factory_test.cc)
add_executable(health_handler_factory_test tests/health_handler_factory_test.cc)
add_executable(echo_handler_factory_test tests/echo_handler_factory_test.cc)

target_link_libraries(config_parser_test config_parser_lib Boost::log_setup Boost::log gtest_main gmock_main)
target_link_libraries(reply_test reply_lib gtest_main gmock_main)
target_link_libraries(session_test session_lib reply_lib Boost::system Boost::regex Boost::log_setup Boost::log gtest_main gmock_main)
target_link_libraries(static_handler_test static_handler_lib config_parser_lib reply_lib Boost::log_setup Boost::log gtest_main gmock_main)
target_link_libraries(echo_handler_test echo_handler_lib reply_lib Boost::system Boost::regex Boost::log_setup Boost::log gtest_main gmock_main)
target_link_libraries(crud_handler_test crud_handler_lib mock_filesystem_lib config_parser_lib Boost::system Boost::regex Boost::log_setup Boost::log gtest_main gmock_main)
target_link_libraries(sleep_handler_test sleep_handler_lib Boost::system Boost::regex Boost::log_setup Boost::log gtest_main gmock_main)
target_link_libraries(health_handler_test health_handler_lib Boost::system Boost::regex Boost::log_setup Boost::log gtest_main gmock_main)
target_link_libraries(not_found_handler_test not_found_handler_lib Boost::system Boost::regex Boost::log_setup Boost::log gtest_main gmock_main)
target_link_libraries(error_handler_test error_handler_lib Boost::system Boost::regex Boost::log_setup Boost::log gtest_main gmock_main)
target_link_libraries(sleep_handler_factory_test sleep_handler_factory_lib gtest_main gmock_main)
target_link_libraries(health_handler_factory_test health_handler_factory_lib gtest_main gmock_main)
target_link_libraries(echo_handler_factory_test echo_handler_factory_lib gtest_main gmock_main)

# Test binary
# gtest_discover_tests(example_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(reply_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(static_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(echo_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(crud_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(sleep_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(health_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(not_found_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(error_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(sleep_handler_factory_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(health_handler_factory_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(echo_handler_factory_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

#integration Test

if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Coverage")
add_test(NAME integration_tests COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
add_test(NAME crud_integration_tests COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/crud_integration_test/crud_integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()


#Generating coverage report
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS config_parser_lib reply_lib session_lib static_handler_lib echo_handler_lib sleep_handler_lib health_handler_lib not_found_handler_lib error_handler_lib sleep_handler_factory_lib health_handler_factory_lib echo_handler_factory_lib TESTS static_handler_test config_parser_test reply_test session_test echo_handler_test crud_handler_test sleep_handler_test health_handler_test not_found_handler_test error_handler_test sleep_handler_factory_test health_handler_factory_test echo_handler_factory_test)
