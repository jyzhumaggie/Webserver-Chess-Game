#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "error_handler.h"

using namespace testing;
using namespace http::server; 

class ErrorHandlerTestFixture : public ::testing::Test {
 protected:
};


TEST_F(ErrorHandlerTestFixture, MalformedRequest) {
    boost::beast::http::request<boost::beast::http::dynamic_body> request_;
    request_.target("bha");

    boost::beast::http::response<boost::beast::http::dynamic_body> response;
    error_handler handler("bha", "test url");

    boost::beast::http::status status_ = handler.serve(request_, response);
    
    std::vector<std::pair<std::string, std::string>> headers;
    for(auto const& field : response)
    {
        std::pair<std::string, std::string> header;
        header.first = std::string(field.name_string());
        header.second = std::string(field.value());
        headers.push_back(header);
    }
    bool success = (response.result() == boost::beast::http::status::bad_request &&
                  headers.at(1).first == "Content-Type" &&
                  headers.at(1).second == "text/plain" &&
                  status_ == response.result());

    EXPECT_TRUE(success);
}