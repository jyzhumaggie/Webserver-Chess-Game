#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>
#include "request_handler.h"
#include "header.h"
#include <boost/asio.hpp>
#include <iostream>
#include <unordered_map>
#include "config_parser.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

using namespace http::server;
using boost::asio::ip::tcp;

/** 
 * file_handler class
 *
 * Inherits from request_handler
 *
 * The file_handler constructs proper echo response checks to ensure that passed in
 * file path is correct based on list of acceptable mime file types
 *
 * The handle_request method constructs the proper response for the given file path.
 * The constructor checks to confirm that the given file path is valid.
 * If so, it return the response with proper headers and status code.
 * If not, it returns an error block with the appropriate status codes, header, and content
 * response given a request object and a root file path.
 *
 */

class file_handler : public request_handler {
    public:
        file_handler(std::string location, std::string request_url, NginxConfig& config);
        beast::http::status serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res);
        std::string get_reply(int err_code);

    private:
        std::string location_;
        std::string request_url_;
        NginxConfig config_;

};


#endif