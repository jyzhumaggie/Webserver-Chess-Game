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

    tcp::socket& socket();

    bool start();

    std::string handle_read(const boost::system::error_code& error, size_t bytes_transferred);

    bool handle_write(const boost::system::error_code& error);

    boost::asio::streambuf request_;

private:
    tcp::socket socket_;

    enum { max_length = 1024 };
};

#endif