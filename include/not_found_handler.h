#ifndef NOT_FOUND_HANDLER_H
#define NOT_FOUND_HANDLER_H

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


class not_found_handler : public request_handler {
    public:
        not_found_handler(std::string location, std::string request_url);
        beast::http::status serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res);

};

#endif