#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>
#include "request_handler.h"
#include "header.h"
#include <boost/asio.hpp>
#include <iostream>

using namespace http::server;

class file_handler : public request_handler {
    public:
        file_handler(std::string base_dir);
        virtual reply handle_request();
};

#endif