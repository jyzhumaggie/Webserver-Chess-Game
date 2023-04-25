#include "gtest/gtest.h"
#include "reply.h"
#include "gmock/gmock.h"

using namespace testing;
using namespace http::server; 

class ReplyTestFixture : public ::testing::Test {
 protected:
  reply rep_ok = reply::stock_reply(reply::ok);
  reply rep_no_content = reply::stock_reply(reply::no_content);
  reply rep_not_found = reply::stock_reply(reply::not_found);
  reply rep_internal_server_error = reply::stock_reply(reply::internal_server_error);
};

TEST_F(ReplyTestFixture, OkStatus) {
  EXPECT_TRUE(rep_ok.status == reply::ok);
}

TEST_F(ReplyTestFixture, NumHeaders) {
  EXPECT_TRUE(rep_ok.headers.size() == 2);
}

TEST_F(ReplyTestFixture, EqualsTrue) {
  EXPECT_TRUE(rep_ok.status == reply::ok);
}

TEST_F(ReplyTestFixture, NoContentStatus) {
  EXPECT_TRUE(rep_no_content.status == reply::no_content);
}

TEST_F(ReplyTestFixture, NoContentHeader0) {
  EXPECT_TRUE(rep_no_content.headers[0].name == "Content-Length");
}

TEST_F(ReplyTestFixture, NotFoundHeader1) {
  EXPECT_TRUE(rep_not_found.headers[1].name == "Content-Type");
}

TEST_F(ReplyTestFixture, HandleError) {
  EXPECT_TRUE(rep_internal_server_error.status == reply::internal_server_error);
}