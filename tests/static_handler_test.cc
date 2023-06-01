#include "gtest/gtest.h"
#include "static_handler.h"
#include "gmock/gmock.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "config_parser.h"

using namespace testing;
using namespace http::server; 

class StaticHandlerTestFixture : public ::testing::Test {
 protected:
    NginxConfig config_;
    std::vector<path> paths;
    std::string location_;
    std::string request_url_;
};

//TODO FIX AFTER IMPLEMNENTATION CHANGED

TEST_F(StaticHandlerTestFixture, GetReply200) {
  
    static_handler handler("/static1", "../files/static/", config_);
    std::string return_string = handler.get_reply(200);
    EXPECT_EQ(return_string, "");
}

TEST_F(StaticHandlerTestFixture, GetReply400) {
    static_handler handler("/static1", "../files/static/", config_);
    std::string expected_output = "<html>"
                                "<head><title>Bad Request</title></head>"
                                "<body><h1>400 Bad Request</h1></body>"
                                "</html>";
    std::string return_string = handler.get_reply(400);
    EXPECT_EQ(return_string, expected_output);
}

TEST_F(StaticHandlerTestFixture, GetReplyOthers) {
    static_handler handler("/static1", "../files/static/", config_);
    std::string expected_output = "<html>"
                                "<head><title>Not Found</title></head>"
                                "<body><h1>404 Not Found</h1></body>"
                                "</html>";
    std::string return_string = handler.get_reply(0);
    EXPECT_EQ(return_string, expected_output);
}