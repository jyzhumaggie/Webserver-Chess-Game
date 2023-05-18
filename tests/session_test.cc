#include "gtest/gtest.h"
#include "session.h"
#include "gmock/gmock.h"

class SessionFixture : public ::testing::Test {
 protected:
    boost::asio::io_service io_service;
    NginxConfig config_;
    std::map<std::string, request_handler_factory*> routes_;

 };


//TODO FIX AFTER IMPLEMENTATION CHANGED

// Tests handle read with an error
// TEST_F(SessionFixture, HandleReadError)
// {
//     session s(io_service, config_, routes_);
//     std::ostream os(&s.request_);
//     std::string testString = "test\r\n\r\n";
//     os<<testString;
//     boost::system::error_code error = make_error_code(boost::system::errc::timed_out);
//     std::string result = s.handle_read(error,testString.size());
//     EXPECT_FALSE(result == testString);
// }

//Tests handle write with an error
//TEST_F(SessionFixture, HandleWriteError){
 //   session s(io_service, config_, routes_);
//  boost::system::error_code error = make_error_code(boost::system::errc::timed_out);
//  EXPECT_FALSE(s.handle_write(error));
//}

//Tests handle write with no error works properly
//TEST_F(SessionFixture, HandleWrite){
//  session s(io_service, config_, routes_);
//  EXPECT_TRUE(s.handle_write(boost::system::error_code()));
//}

//Tests session starts successfully
TEST_F(SessionFixture, SessionStart){
    session s(io_service, config_, routes_);
  std::vector<path> paths;
  EXPECT_TRUE(s.start(paths));
}


