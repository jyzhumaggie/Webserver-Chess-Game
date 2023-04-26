#include "gtest/gtest.h"
#include "session.h"
#include "gmock/gmock.h"

class SessionFixture : public ::testing::Test {
 protected:
    boost::asio::io_service io_service;
};


TEST_F(SessionFixture, HandleReadCarriage)
{
    session s(io_service);
    std::ostream os(&s.request_);
    std::string testString = "test\r\n\r\n";
    os<<testString;
    std::string result = s.handle_read(boost::system::error_code(),testString.size());
    EXPECT_TRUE(testString == result);
}


TEST_F(SessionFixture, HandleReadNormal)
{
    session s(io_service);
    std::ostream os(&s.request_);
    std::string testString = "test\n\n";
    os<<testString;
    std::string result = s.handle_read(boost::system::error_code(),testString.size());
    EXPECT_TRUE(testString == result);
}



TEST_F(SessionFixture, HandleReadError)
{
    session s(io_service);
    std::ostream os(&s.request_);
    std::string testString = "test\r\n\r\n";
    os<<testString;
    boost::system::error_code error = make_error_code(boost::system::errc::timed_out);
    std::string result = s.handle_read(error,testString.size());
    EXPECT_FALSE(result == testString);
}


TEST_F(SessionFixture, HandleWriteError){
  session s(io_service);
  boost::system::error_code error = make_error_code(boost::system::errc::timed_out);
  EXPECT_FALSE(s.handle_write(error));
}


TEST_F(SessionFixture, HandleWrite){
  session s(io_service);
  EXPECT_TRUE(s.handle_write(boost::system::error_code()));
}


TEST_F(SessionFixture, SessionStart){
  session s(io_service);
  EXPECT_TRUE(s.start());
}


