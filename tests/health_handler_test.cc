#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "health_handler.h"

using namespace testing;
using namespace http::server; 

class HealthHandlerTestFixture : public ::testing::Test {
 protected:
};


TEST_F(HealthHandlerTestFixture, HandleCorrectRequest) {
    boost::beast::http::request<boost::beast::http::dynamic_body> request_;
    request_.target("/health");

    std::string expected_output = "OK";

    boost::beast::http::response<boost::beast::http::dynamic_body> response;
    health_handler handler("/health", "test url");

    boost::beast::http::status status_ = handler.serve(request_, response);
    std::string body { boost::asio::buffers_begin(response.body().data()),
                     boost::asio::buffers_end(response.body().data()) };

    std::vector<std::pair<std::string, std::string>> headers;
    for(auto const& field : response)
    {
        std::pair<std::string, std::string> header;
        header.first = std::string(field.name_string());
        header.second = std::string(field.value());
        headers.push_back(header);
    }
    bool success = (response.result() == boost::beast::http::status::ok &&
                  body == expected_output &&
                  response.has_content_length() &&
                  headers.at(1).first == "Content-Type" &&
                  headers.at(1).second == "text/plain" &&
                  status_ == response.result());

    EXPECT_TRUE(success);
}