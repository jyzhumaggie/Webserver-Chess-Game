#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "echo_handler_factory.h"

class EchoHandlerFactoryTestFixture : public ::testing::Test {
 protected:
};

TEST_F(EchoHandlerFactoryTestFixture, CreateTest) {

    NginxConfig config;

    echo_handler_factory factory("test_path",config); 

    // Create a test location and url
    std::string location = "test_location";
    std::string url = "test_url";

    // Call the create function and check if it returns a valid pointer
    request_handler* handler = factory.create(location, url);
    EXPECT_NE(handler, nullptr);

    // Clean up the created handler
    delete handler;
}