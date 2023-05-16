#ifndef STATIC_HANDLER_FACTORY_H
#define STATIC_HANDLER_FACTORY_H

#include "request_handler_factory.h"
#include <string>
#include "config_parser.h"
#include "request_handler.h"
#include "file_handler.h"


class static_handler_factory : public request_handler_factory {
    public:
    static_handler_factory(const std::string& path, NginxConfig& config);
    request_handler* create(const std::string& location,NginxConfig& config);
};


#endif