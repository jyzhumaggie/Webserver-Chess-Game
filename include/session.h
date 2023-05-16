#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

// #include "session.h"
#include "reply.h"
#include "config_parser.h"
#include "request_handler_factory.h"
#include "echo_handler_factory.h"

using boost::asio::ip::tcp;
using http::server::reply;

class session
{
public:
    session(boost::asio::io_service& io_service, NginxConfig& config, std::map<std::string, request_handler_factory*> routes);

    tcp::socket& socket();

    bool start(std::vector<path> paths);

    std::string handle_read(const boost::system::error_code& error, size_t bytes_transferred);

    bool handle_write(const boost::system::error_code& error);

    boost::asio::streambuf request_;

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