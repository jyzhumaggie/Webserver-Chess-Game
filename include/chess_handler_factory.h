#ifndef CHESS_HANDLER_FACTORY_H
#define CHESS_HANDLER_FACTORY_H

#include "request_handler_factory.h"
#include <string>
#include "config_parser.h"
#include "request_handler.h"
#include "chess_handler.h"

/**
 * chess_handler_factory class inherits from request_handler_factory class
 *
 * the chess handler factory class constructs a chess handler factory object by giving the 
 * config file and path
 *
 * the create function will return a request_handler point, which in this class points to an chess handler 
 */

class chess_handler_factory : public request_handler_factory {
    public:
    chess_handler_factory(const std::string& path, NginxConfig& config);
    request_handler* create(const std::string& location, const std::string& url);
    
    private:
        std::string location_;
        NginxConfig config_;
};


#endif