#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"
#include "reply.h"

using boost::asio::ip::tcp;
using http::server::reply;

class session
{
public:
    session(boost::asio::io_service& io_service);

    reply create_reply(std::string complete_request, int bytes_transferred);

    tcp::socket& socket();

    void start();

private:
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_write(const boost::system::error_code& error);

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    boost::asio::streambuf request_;
};

#endif