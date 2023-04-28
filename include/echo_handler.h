#ifndef ECHO_HANDLER_H
#define ECHO_HANDLER_H

#include <string>
#include <vector>
#include "request_handler.h"
#include "header.h"
#include <boost/asio.hpp>
#include <iostream>

using namespace http::server;

class echo_handler : public request_handler {
    public:
        echo_handler(std::string base_dir);
        virtual reply handle_request();
};

#endif