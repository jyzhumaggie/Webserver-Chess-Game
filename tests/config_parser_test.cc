#include "gtest/gtest.h"
#include "config_parser.h"

class NginxConfigParserTest : public ::testing::Test {
 protected:
  NginxConfig out_config;
  NginxConfigParser parser;
};

TEST_F(NginxConfigParserTest, SimpleConfig) {
  bool success = parser.Parse("example_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, AdjacentCurlyBraces) {
  bool success = parser.Parse("example_config1", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, ConfigWithoutStatements) {
  bool success = parser.Parse("example_config2", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, EmptyConfig) {
  bool success = parser.Parse("example_config3", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, InvalidBraces) {
  bool success = parser.Parse("example_config4", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, SpaceBeforeColon) {
  bool success = parser.Parse("example_config5", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, NestedBlocks) {
  bool success = parser.Parse("example_config6", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, StatementWithoutToken) {
  bool success = parser.Parse("example_config7", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, EscapeWithinQuotedStrings) {
  bool success = parser.Parse("example_config8", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, InvalidLeftParenthesis) {
  bool success = parser.Parse("example_config9", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, Comments) {
  bool success = parser.Parse("example_config10", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, StatementWithMultipleToken) {
  bool success = parser.Parse("example_config11", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, QuotedStringsFollowedByWhitespace) {
  bool success = parser.Parse("example_config12", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, ConfigWithMultipleStatements) {
  bool success = parser.Parse("example_config13", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, NestLoopConfig) {
  bool success = parser.Parse("nested_loop_config", &out_config);
  std::string config_string = out_config.ToString();
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, EmptyBlock) {
  bool success = parser.Parse("empty_block", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, InvalidRightParenthesisConfig) {
  bool success = parser.Parse("invalid_right_parenthesis_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, UnexistConfig) {
  bool success = parser.Parse("unexisit_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, NginxConfigToStringNested) {
  bool success = parser.Parse("nested_loop_config", &out_config);

  std::string res =  "foo \"bar\";\nserver {\n  location / {\n    proxy_pass http://localhost:8080;\n  }\n}\n";
  std::string config_string = out_config.ToString(0);
  bool same = config_string.compare(res)==0;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, GetPortNumberChildBlock) {
  bool success = parser.Parse("example_config", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==8080;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, InvalidLeftParenthesisConfig) {
  bool success = parser.Parse("invalid_left_parenthesis_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, GetPortNumber) {
  bool success = parser.Parse("test_port_number", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==80;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, GetPortNumberNull) {
  bool success = parser.Parse("empty_config", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, NestedPort) {
  bool success = parser.Parse("nested_port", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==8080;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, InvalidPort) {
  bool success = parser.Parse("invalid_port", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, LargePortNumber) {
  bool success = parser.Parse("large_port_config", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, NegativePortNumber) {
  bool success = parser.Parse("negative_port_config", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, GetPortNumberNoFile) {
  bool success = parser.Parse("unexist_config", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, GetPortNumberNullptr) {
  bool success = parser.Parse("example_config", &out_config);

  int port = out_config.get_port_from_config(NULL);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, GetPortNumberNoPort) {
  bool success = parser.Parse("no_port_config", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, SingleQuote) {
  bool success = parser.Parse("single_quote", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, DoubleQuote) {
  bool success = parser.Parse("double_quote", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, SingleQuoteError) {
  bool success = parser.Parse("single_quote_error", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, DoubleQuoteError) {
  bool success = parser.Parse("double_quote_error", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, StartBlockError) {
  bool success = parser.Parse("start_block_error", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, StatementEndError) {
  bool success = parser.Parse("statement_end_error", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, EndBlockError) {
  bool success = parser.Parse("end_block_error", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, EofSingleQuote) {
  bool success = parser.Parse("endl_single_quote", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, EofDoubleQuote) {
  bool success = parser.Parse("endl_double_quote", &out_config);
  EXPECT_FALSE(success);
}