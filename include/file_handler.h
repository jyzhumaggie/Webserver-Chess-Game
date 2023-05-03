#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>
#include "request_handler.h"
#include "header.h"
#include <boost/asio.hpp>
#include <iostream>
#include <unordered_map>

using namespace http::server;
using boost::asio::ip::tcp;

class file_handler : public request_handler {
    public:
        file_handler(std::string base_dir);
        void handle_request(tcp::socket& socket);
    private:
        std::unordered_map<std::string, std::string> ext_to_mime;
};

#endif