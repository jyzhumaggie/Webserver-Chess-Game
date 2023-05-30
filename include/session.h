#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include "reply.h"
#include "config_parser.h"
#include "request_handler_factory.h"
#include "health_handler_factory.h"
#include "echo_handler_factory.h"
#include "error_handler.h"

using boost::asio::ip::tcp;
using http::server::reply;

/**
 * Session class
 * 
 * Session objects contain socket information and
 * data that is passed from a read or write operation
 *
 * The functions handle_read and handle_write both use
 * the boost library to write to a designated socket or read from one respectively
 */

class session
{
public:
    session(boost::asio::io_service& io_service, NginxConfig& config, std::map<std::string, request_handler_factory*> routes);

    tcp::socket& socket();

    bool start(std::vector<path> paths);

    bool is_request_complete(const boost::beast::error_code& ec, std::size_t bytes_transferred);
    bool handle_read(const boost::system::error_code& error, size_t bytes_transferred);

    bool handle_write(const boost::system::error_code& error);

    boost::beast::http::request<boost::beast::http::dynamic_body> request_;
    boost::asio::streambuf buffer_;

    std::string match(std::map<std::string, request_handler_factory*> routes, std::string& url);
    bool set_routes(std::map<std::string, request_handler_factory*> routes);
    
private:
    tcp::socket socket_;
    std::string clientIP_;
    std::vector<path> paths_;
    enum { max_length = 1024 };
    NginxConfig config_;
    std::map<std::string, request_handler_factory*> routes_;
};

#endif