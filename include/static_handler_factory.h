// #ifndef STATIC_HANDLER_FACTORY_H
// #define STATIC_HANDLER_FACTORY_H

// #include "request_handler_factory.h"
// #include "request_handler.h"


// // #include "file_handler.h"

// /**
//  * static_handler_factory class
//  *
//  * inherits from request_handler_factory
//  * 
//  * The static handler factory constructs a static handler factory
//  * by giving the location and an nginx config file
//  *
//  * The function create will return a request_handler point.
//  * In this class, the return point is a static handler.
//  */

// class static_handler_factory : public request_handler_factory {
//     public:
//     static_handler_factory(const std::string& path, NginxConfig& config);
//     // request_handler* create(const std::string& location,NginxConfig& config);
//     request_handler* create(const std::string& location, const std::string& url);
    
//     private:
//         std::string location_;
//         NginxConfig config_;

// };

// #endif

#ifndef STATIC_HANDLER_FACTORY_H
#define STATIC_HANDLER_FACTORY_H

#include "request_handler_factory.h"
#include <string>
#include "config_parser.h"
#include "request_handler.h"
#include "echo_handler.h"
#include "file_handler.h"

/**
 * echo_handler_factory class inherits from request_handler_factory class
 *
 * the echo handler factory class constructs an echo handler factory object by giving the 
 * config file and path
 *
 * the create function will return a request_handler point, which in this class points to an echo handler 
 */

class static_handler_factory : public request_handler_factory {
    public:
    static_handler_factory(const std::string& path, NginxConfig& config);
    request_handler* create(const std::string& location, const std::string& url);
    
    private:
        std::string location_;
        NginxConfig config_;
};


#endif