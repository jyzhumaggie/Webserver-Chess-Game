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
        file_handler(std::string base_dir,std::vector<path> paths);
        void handle_request(tcp::socket& socket);
        
    private:
        std::unordered_map<std::string, std::string> ext_to_mime;
};

#endif