#ifndef STATIC_HANDLER_FACTORY_H
#define STATIC_HANDLER_FACTORY_H

#include "request_handler_factory.h"
#include <string>
#include "config_parser.h"

class static_handler_factory : public request_handler_factory {
    public:
    static_handler_factory(const std::string& path, NginxConfig& config);
};


#endif