#ifndef SLEEP_HANDLER_H
#define SLEEP_HANDLER_H

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
const static int SLEEP_TIME = 5000000; // in microsecond
/**
 * sleep_handler class
 *
 * Inherits from request_handler class
 *
 * The sleep handler constructs proper response about sleep info 
 *
 * The function handle_request constructs the response 
 * with the proper status codes, content, and headers.
 */

class sleep_handler : public request_handler {
    public:
        sleep_handler(string location, string request_url);
        beast::http::status serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res);

};

#endif