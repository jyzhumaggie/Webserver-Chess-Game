#ifndef ECHO_HANDLER_FACTORY_H
#define ECHO_HANDLER_FACTORY_H

#include "request_handler_factory.h"
#include <string>
#include "config_parser.h"
#include "request_handler.h"
#include "echo_handler.h"

/**
 * echo_handler_factory class inherits from request_handler_factory class
 *
 * the echo handler factory class constructs an echo handler factory object by giving the 
 * config file and path
 *
 * the create function will return a request_handler point, which in this class points to an echo handler 
 */

class echo_handler_factory : public request_handler_factory {
    public:
    echo_handler_factory(const std::string& path, NginxConfig& config);
    request_handler* create(const std::string& location, const std::string& url);
    
    private:
        std::string location_;
        NginxConfig config_;
};


#endif