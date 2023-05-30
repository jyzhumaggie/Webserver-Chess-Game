#ifndef HEALTH_HANDLER_H
#define HEALTH_HANDLER_H

#include <string>
#include <vector>
#include "request_handler.h"
#include "header.h"
#include "reply.h"

#include <boost/asio.hpp>
#include <iostream>



using namespace http::server;
using boost::asio::ip::tcp;
using namespace std;

/**
 * health_handler class
 *
 * Inherits from health_handler class
 *
 * The health handler constructs proper response about health info 
 *
 * The function handle_request constructs the response 
 * with the proper status codes, content, and headers.
 */

class health_handler : public request_handler {
    public:
        health_handler(string location, string request_url);
        beast::http::status serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res);

};

#endif