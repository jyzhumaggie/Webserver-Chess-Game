#ifndef CHESS_HANDLER_H
#define CHESS_HANDLER_H

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
 * chess_handler class
 *
 * Inherits from chess_handler class
 *
 * The chess handler listens on the "/chess/" datapath, and takes in a FEN string afterwards
 *
 * The function handle_request constructs the response 
 * with the proper status codes, content, and headers.
 */

class chess_handler : public request_handler {
    public:
        chess_handler(string location, string request_url);
        beast::http::status serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res);
    private:
        std::string get_fen(std::string location, std::string request_url);
        beast::http::status generate_error_response(beast::http::response<beast::http::dynamic_body>& res, beast::http::status s, std::string msg);
        std::string location_;
        std::string request_url_;
};

#endif