#ifndef ECHO_HANDLER_H
#define ECHO_HANDLER_H

#include <string>
#include <vector>
#include "request_handler.h"
#include "header.h"
#include "reply.h"

#include <boost/asio.hpp>
#include <iostream>

using namespace http::server;
using boost::asio::ip::tcp;

/**
 * echo_handler class
 *
 * Inherits from request_handler class
 *
 * The echo handler constructs proper echo response based on root directory 
 * and vector of paths that is provided to it
 *
 * The function handle_request constructs the echo response
 * with the proper status codes, content, and headers.
 */

class echo_handler : public request_handler {
    public:
        echo_handler(std::string base_dir);
        void handle_request(tcp::socket& socket);
};

#endif