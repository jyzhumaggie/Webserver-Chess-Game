#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "health_handler_factory.h"

class HealthHandlerFactoryTestFixture : public ::testing::Test {
 protected:
};

TEST_F(HealthHandlerFactoryTestFixture, CreateTest) {

    NginxConfig config;

    health_handler_factory factory("test_path",config); 

    // Create a test location and url
    std::string location = "test_location";
    std::string url = "test_url";

    // Call the create function and check if it returns a valid pointer
    request_handler* handler = factory.create(location, url);
    EXPECT_NE(handler, nullptr);

    // Clean up the created handler
    delete handler;
}