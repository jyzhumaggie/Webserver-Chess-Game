#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <string>
#include <vector>
#include "header.h"
#include "reply.h"
#include <boost/asio.hpp>
#include <iostream>
#include "config_parser.h"

using namespace http::server;
using boost::asio::ip::tcp;

class request_handler {
    public:
        request_handler(std::string base_dir,std::vector<path> paths);
        bool parse(std::string request); //true if successfully parsed
        //need to make parse more robust - make sure method is valid (GET), path exists (and get content type)
        virtual void handle_request(tcp::socket& socket) = 0; 
        
    protected:
        std::vector<std::string> get_lines(std::string input); //Vector of input with delimeter \n or \r\n
        std::vector<std::string> get_tokens_from_request_line(std::string request_line);
        bool valid_header(std::string header);
        header get_header(std::string header);
        std::string get_filename(std::string root_path); //taken from after the last '/' in the path
        std::string get_extension(std::string filename); //taken from after the first '.' in the filename (e.g. file.tar.gz -> tar.gz)

        std::string complete_request;
        std::string method;
        std::string root_path;
        std::string http_type;
        std::string filename;
        std::string extension;
        std::vector<header> headers;
        std::string content;

        std::vector<path> paths_;
};

#endif