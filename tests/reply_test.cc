#include "gtest/gtest.h"
#include "reply.h"
#include "gmock/gmock.h"

using namespace testing;
using namespace http::server; 

class ReplyTestFixture : public ::testing::Test {
 protected:
  std::vector<reply::status_type> statuses = {reply::ok, reply::created, reply::accepted, reply::no_content, reply::multiple_choices, reply::moved_permanently, reply::moved_temporarily,
                                              reply::not_modified, reply::bad_request, reply::unauthorized, reply::forbidden, reply::not_found, reply::internal_server_error,
                                              reply::not_implemented, reply::bad_gateway, reply::service_unavailable};
  
  std::vector<reply> stock_replies = {};
  std::vector<std::vector<boost::asio::const_buffer>> stock_buffers = {};

  bool cmp(boost::asio::const_buffer lhs, boost::asio::const_buffer rhs) {
    return boost::asio::buffer_cast<const unsigned char*>(lhs) == boost::asio::buffer_cast<const unsigned char*>(rhs);
  }

  virtual void SetUp() {
    for (std::vector<reply::status_type>::iterator itr = statuses.begin(); itr != statuses.end(); itr++) {
        stock_replies.push_back(reply::stock_reply(*itr));
    }
    for (std::vector<reply>::iterator itr = stock_replies.begin(); itr != stock_replies.end(); itr++) {
        stock_buffers.push_back(itr->to_buffers());
    }
  }
};

TEST_F(ReplyTestFixture, CheckReplyStatus) {
  for (int i = 0; i < statuses.size(); i++) {
    EXPECT_TRUE(stock_replies[i].status == statuses[i]);
  }
}

TEST_F(ReplyTestFixture, CheckReplyHeaderSize) {
  for (int i = 0; i < stock_replies.size(); i++) {
    EXPECT_TRUE(stock_replies[i].headers.size() == 2);
  }
}

TEST_F(ReplyTestFixture, CheckReplyContent) {
  for (int i = 0; i < stock_replies.size(); i++) {
    EXPECT_TRUE(stock_replies[i].content == stock_replies::to_string(statuses[i]));
  }
}

TEST_F(ReplyTestFixture, CheckReplyFirstHeaderName) {
  for (int i = 0; i < stock_replies.size(); i++) {
    EXPECT_TRUE(stock_replies[i].headers[0].name == "Content-Length");
  }
}

TEST_F(ReplyTestFixture, CheckReplyFirstHeaderValue) {
  for (int i = 0; i < stock_replies.size(); i++) {
    EXPECT_TRUE(stock_replies[i].headers[0].value == std::to_string(stock_replies[i].content.size()));
  }
}

TEST_F(ReplyTestFixture, CheckReplySecondHeaderName) {
  for (int i = 0; i < stock_replies.size(); i++) {
    EXPECT_TRUE(stock_replies[i].headers[1].name == "Content-Type");
  }
}

TEST_F(ReplyTestFixture, CheckReplySecondHeaderValue) {
  for (int i = 0; i < statuses.size(); i++) {
    EXPECT_TRUE(stock_replies[i].headers[1].value == "text/html");
  }
}

TEST_F(ReplyTestFixture, CheckBuffersLength) {
  for (int i = 0; i < stock_buffers.size(); i++) {
    EXPECT_TRUE(stock_buffers[i].size() == 11);
  }
}

TEST_F(ReplyTestFixture, CheckBuffersStatus) {
  for (int i = 0; i < stock_buffers.size(); i++) {
    EXPECT_TRUE(cmp(stock_buffers[i][0], status_strings::to_buffer(statuses[i])));
  }
}

TEST_F(ReplyTestFixture, CheckBuffersFirstHeader) {
  for (int i = 0; i < stock_replies.size(); i++) {
    header &h = stock_replies[i].headers[0];
    EXPECT_TRUE(cmp(stock_buffers[i][1], boost::asio::buffer(h.name)));
    EXPECT_TRUE(cmp(stock_buffers[i][3], boost::asio::buffer(h.value)));
  }
}

TEST_F(ReplyTestFixture, CheckBuffersSecondHeader) {
  for (int i = 0; i < stock_replies.size(); i++) {
    header &h = stock_replies[i].headers[1];
    EXPECT_TRUE(cmp(stock_buffers[i][5], boost::asio::buffer(h.name)));
    EXPECT_TRUE(cmp(stock_buffers[i][7], boost::asio::buffer(h.value)));
  }
}

TEST_F(ReplyTestFixture, CheckBuffersContent) {
  for (int i = 0; i < stock_replies.size(); i++) {
    EXPECT_TRUE(cmp(stock_buffers[i][10], boost::asio::buffer(stock_replies[i].content)));
  }
}