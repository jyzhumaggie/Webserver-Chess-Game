#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "not_found_handler.h"

using namespace testing;
using namespace http::server; 

class NotFoundHandlerTestFixture : public ::testing::Test {
 protected:
};


TEST_F(NotFoundHandlerTestFixture, NonExistingPath) {
    boost::beast::http::request<boost::beast::http::dynamic_body> request_;
    request_.target("/bha");

    boost::beast::http::response<boost::beast::http::dynamic_body> response;
    not_found_handler handler("/bha", "test url");

    boost::beast::http::status status_ = handler.serve(request_, response);
    
    std::vector<std::pair<std::string, std::string>> headers;
    for(auto const& field : response)
    {
        std::pair<std::string, std::string> header;
        header.first = std::string(field.name_string());
        header.second = std::string(field.value());
        headers.push_back(header);
    }
    bool success = (response.result() == boost::beast::http::status::not_found &&
                  headers.at(1).first == "Content-Type" &&
                  headers.at(1).second == "text/plain" &&
                  status_ == response.result());
    EXPECT_TRUE(success);
}