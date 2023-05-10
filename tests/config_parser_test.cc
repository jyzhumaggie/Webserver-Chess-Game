#include "gtest/gtest.h"
#include "config_parser.h"

class NginxConfigParserTest : public ::testing::Test {
 protected:
  NginxConfig out_config;
  NginxConfigParser parser;
};

TEST_F(NginxConfigParserTest, SimpleConfig) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, AdjacentCurlyBraces) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config1", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, ConfigWithoutStatements) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config2", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, EmptyConfig) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config3", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, InvalidBraces) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config4", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, SpaceBeforeColon) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config5", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, NestedBlocks) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config6", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, StatementWithoutToken) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config7", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, EscapeWithinQuotedStrings) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config8", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, InvalidLeftParenthesis) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config9", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, Comments) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config10", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, StatementWithMultipleToken) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config11", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, QuotedStringsFollowedByWhitespace) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config12", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, ConfigWithMultipleStatements) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config13", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, NestLoopConfig) {
  bool success = parser.Parse("ConfigParserTestFiles/nested_loop_config", &out_config);
  std::string config_string = out_config.ToString();
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, EmptyBlock) {
  bool success = parser.Parse("ConfigParserTestFiles/empty_block", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, InvalidRightParenthesisConfig) {
  bool success = parser.Parse("ConfigParserTestFiles/invalid_right_parenthesis_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, UnexistConfig) {
  bool success = parser.Parse("ConfigParserTestFiles/unexisit_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, NginxConfigToStringNested) {
  bool success = parser.Parse("ConfigParserTestFiles/nested_loop_config", &out_config);

  std::string res =  "foo \"bar\";\nserver {\n  location / {\n    proxy_pass http://localhost:8080;\n  }\n}\n";
  std::string config_string = out_config.ToString(0);
  bool same = config_string.compare(res)==0;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, GetPortNumberChildBlock) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==8080;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, InvalidLeftParenthesisConfig) {
  bool success = parser.Parse("ConfigParserTestFiles/invalid_left_parenthesis_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, GetPortNumber) {
  bool success = parser.Parse("ConfigParserTestFiles/test_port_number", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==80;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, GetPortNumberNull) {
  bool success = parser.Parse("ConfigParserTestFiles/empty_config", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, NestedPort) {
  bool success = parser.Parse("ConfigParserTestFiles/nested_port", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==8080;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, InvalidPort) {
  bool success = parser.Parse("ConfigParserTestFiles/invalid_port", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, LargePortNumber) {
  bool success = parser.Parse("ConfigParserTestFiles/large_port_config", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, NegativePortNumber) {
  bool success = parser.Parse("ConfigParserTestFiles/negative_port_config", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, GetPortNumberNoFile) {
  bool success = parser.Parse("ConfigParserTestFiles/unexist_config", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, GetPortNumberNullptr) {
  bool success = parser.Parse("ConfigParserTestFiles/example_config", &out_config);

  int port = out_config.get_port_from_config(NULL);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, GetPortNumberNoPort) {
  bool success = parser.Parse("ConfigParserTestFiles/no_port_config", &out_config);

  int port = out_config.get_port_from_config(&out_config);
  bool same = port==-1;

  EXPECT_TRUE(same);
}

TEST_F(NginxConfigParserTest, SingleQuote) {
  bool success = parser.Parse("ConfigParserTestFiles/single_quote", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, DoubleQuote) {
  bool success = parser.Parse("ConfigParserTestFiles/double_quote", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, SingleQuoteError) {
  bool success = parser.Parse("ConfigParserTestFiles/single_quote_error", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, DoubleQuoteError) {
  bool success = parser.Parse("ConfigParserTestFiles/double_quote_error", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, StartBlockError) {
  bool success = parser.Parse("ConfigParserTestFiles/start_block_error", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, StatementEndError) {
  bool success = parser.Parse("ConfigParserTestFiles/statement_end_error", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, EndBlockError) {
  bool success = parser.Parse("ConfigParserTestFiles/end_block_error", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, EofSingleQuote) {
  bool success = parser.Parse("ConfigParserTestFiles/endl_single_quote", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, EofDoubleQuote) {
  bool success = parser.Parse("ConfigParserTestFiles/endl_double_quote", &out_config);
  EXPECT_FALSE(success);
}