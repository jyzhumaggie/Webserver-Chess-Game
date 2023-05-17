#ifndef REQUEST_HANDLER_FACTORY_H
#define REQUEST_HANDLER_FACTORY_H

#include <string>
#include <boost/log/trivial.hpp>
#include "config_parser.h"
#include "request_handler.h"

/*
 * Parent class for request handler factory object.
 * Request handler factories are designed to create a request handler 
 * by giving the location and nginx config file.
 */

class request_handler_factory {
    public:
        virtual request_handler* create(const std::string& location, NginxConfig& config) = 0;
};

#endif