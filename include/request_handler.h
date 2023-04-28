#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <string>
#include <vector>
#include "header.h"
#include <boost/asio.hpp>
#include <iostream>

using namespace http::server;

class request_handler {
    public:
        request_handler(std::string base_dir);
        bool parse(std::string request); //true if successfully parsed
        //custom inherited function: serve or smthn
        
    private:
        std::vector<std::string> get_lines(std::string input); //Vector of input with delimeter \n or \r\n
        std::vector<std::string> get_tokens_from_request_line(std::string request_line);
        bool valid_header(std::string header);
        header get_header(std::string header);
        
        std::string method;
        std::string path;
        std::string http_type;
        std::vector<header> headers;
        std::string content;
};

#endif