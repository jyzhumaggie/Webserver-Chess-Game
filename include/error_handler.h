#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

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
 * error_handler class
 *
 * Inherits from request_handler
 *
 * The error handler constructs proper error response based on root directory 
 * and vector of paths that is provided to it
 *
 * The function handle_request returns an error response 
 * with proper headers and status codes
 */

class error_handler : public request_handler {
    public:
        error_handler(std::string base_dir);
        void handle_request(tcp::socket& socket);
};

#endif