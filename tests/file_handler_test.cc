#include "gtest/gtest.h"
#include "file_handler.h"
#include "gmock/gmock.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "config_parser.h"

using namespace testing;
using namespace http::server; 

class FileHandlerTestFixture : public ::testing::Test {
 protected:
};

TEST_F(FileHandlerTestFixture, BrokenPipeException) {
  bool except_found = false;
  try {
    std::vector<path> paths;
    file_handler fh("../files/static/",paths);
    fh.parse("GET test_file.txt HTTP/1.1");
    unsigned short port_num = 3333;
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(), port_num);
    boost::asio::io_service ios;
    boost::asio::ip::tcp::socket sock(ios, ep.protocol());
    boost::system::error_code ec;
    sock.bind(ep, ec);
    fh.handle_request(sock);
  }
  catch (...){
    except_found = true;
  }
  EXPECT_TRUE(except_found);
}

TEST_F(FileHandlerTestFixture, BadFileDescriptorException) {
  bool except_found = false;
  try {
    std::vector<path> paths;
    file_handler fh("",paths);
    fh.parse("");
    unsigned short port_num = 3333;
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(), port_num);
    boost::asio::io_service ios;
    boost::asio::ip::tcp::socket sock(ios, ep.protocol());
    boost::system::error_code ec;
    sock.bind(ep, ec);
    fh.handle_request(sock);
    sock.close();
  }
  catch (...){
    except_found = true;
  }
  EXPECT_TRUE(except_found);
}