#include "gtest/gtest.h"
#include "config_parser.h"
#include "gmock/gmock.h"

using namespace testing;

class MockNginxConfigParser : public NginxConfigParser {
 public:
  MockNginxConfigParser() {  }
  MOCK_METHOD2(ParseToken, TokenType(std::istream* input, std::string* value));
 private:
  NginxConfigParser real_;
};

class NginxConfigParserTestFixture : public ::testing::Test {
 protected:
  NginxConfig out_config;
  NginxConfigParser parser;
};

TEST_F(NginxConfigParserTestFixture, SimpleConfigFixture) {
  bool success = parser.Parse("example_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, AdjacentCurlyBraces) {
  bool success = parser.Parse("example_config1", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, ConfigWithoutStatements) {
  bool success = parser.Parse("example_config2", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, EmptyConfig) {
  bool success = parser.Parse("example_config3", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, InvalidBraces) {
  bool success = parser.Parse("example_config4", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTestFixture, SpaceBeforeColon) {
  bool success = parser.Parse("example_config5", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, NestedBlocks) {
  bool success = parser.Parse("example_config6", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, StatementWithoutToken) {
  bool success = parser.Parse("example_config7", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTestFixture, EscapeWithinQuotedStrings) {
  bool success = parser.Parse("example_config8", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, InvalidLeftParenthesis) {
  bool success = parser.Parse("example_config9", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTestFixture, Comments) {
  bool success = parser.Parse("example_config10", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, StatementWithMultipleToken) {
  bool success = parser.Parse("example_config11", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, QuotedStringsFollowedByWhitespace) {
  bool success = parser.Parse("example_config12", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTestFixture, ConfigWithMultipleStatements) {
  bool success = parser.Parse("example_config13", &out_config);
  EXPECT_TRUE(success);
}