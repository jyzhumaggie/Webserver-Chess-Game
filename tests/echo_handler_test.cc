#include "gtest/gtest.h"
#include "echo_handler.h"
#include "gmock/gmock.h"
#include "config_parser.h"

class echo_handlerFixture : public ::testing::Test {
  protected:
    // boost::asio::io_service io_service;
};

//TODO FIX TESTS AFTER IMPLEMENTATION

// TEST_F(echo_handlerFixture, Handler) {
// 	bool except_found = false;
// 	boost::asio::io_service io_service;
// 	tcp::socket socket(io_service);
// 	try {
// 		echo_handler handler("");
// 		handler.parse("GET\n\n");
// 		handler.handle_request(socket);	
// 	}
// 	catch (...) {
// 		except_found = true;
// 	}
//   		EXPECT_TRUE(except_found); 
// }
