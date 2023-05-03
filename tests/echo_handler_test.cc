#include "gtest/gtest.h"
#include "echo_handler.h"
#include "gmock/gmock.h"
#include "config_parser.h"

class echo_handlerFixture : public ::testing::Test {
  protected:
    // boost::asio::io_service io_service;
	std::vector<path> paths;
};

// TEST_F(echo_handlerFixture, EmptyRequest) {
//     path path1;
//     path1.root = " ";
//     path1.endpoint = " ";
//     paths.push_back(path1);
// 	echo_handler handler("",paths);
//   	EXPECT_FALSE(handler.parse("")); 
// }

// TEST_F(echo_handlerFixture, BadRequestIncorrectSize) {
// 	echo_handler handler("",paths);
//   	EXPECT_FALSE(handler.parse("GET\n\n")); 
// }

// TEST_F(echo_handlerFixture, BadRequestIncorrectMethod) {
// 	echo_handler handler("",paths);
//   	EXPECT_FALSE(handler.parse("POST hello.txt HTTP/1.1\n\n")); 
// }

// TEST_F(echo_handlerFixture, BadRequestIncorrectHTTP) {
// 	echo_handler handler("",paths);
//   	EXPECT_FALSE(handler.parse("GET hello.txt HTTP/1.0\n\n")); 
// }

// TEST_F(echo_handlerFixture, BadRequestNoFileName) {
// 	echo_handler handler("",paths);
//   	EXPECT_FALSE(handler.parse("GET static1/ HTTP/1.1\n\n")); 
// }


// TEST_F(echo_handlerFixture, GoodRequestSingleLine) {
// 	echo_handler handler("",paths);
//   	EXPECT_TRUE(handler.parse("GET /static1/hello.txt HTTP/1.1\n\n")); 
// }

// TEST_F(echo_handlerFixture, GoodRequestMultiline) {
// 	echo_handler handler("",paths);
//   	EXPECT_TRUE(handler.parse("GET /static1/hello.txt HTTP/1.1\nHost: localhost8080\n\n")); 
// }

// TEST_F(echo_handlerFixture, BadRequestMultiline) {
// 	echo_handler handler("",paths);
//   	EXPECT_FALSE(handler.parse("GET /static1/hello.txt HTTP/1.1\nHost: localhost:8080\n\n")); 
// }

// TEST_F(echo_handlerFixture, Handler) {
// 	bool except_found = false;
// 	boost::asio::io_service io_service;
// 	tcp::socket socket(io_service);
// 	echo_handler handler("",paths);
// 	handler.parse("GET\n\n");
// 	// EXPECT_TRUE(handler.parse("GET /static1/hello.txt HTTP/1.1\nHost: localhost8080\n\n")); 
// 	try {
// 		handler.handle_request(socket);	
// 	}
// 	catch (...) {
// 		except_found = true;
// 	}
//   		EXPECT_TRUE(except_found); 
// }
