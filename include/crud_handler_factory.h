#ifndef CRUD_HANDLER_FACTORY_H
#define CRUD_HANDLER_FACTORY_H

#include <string>

#include "config_parser.h"
#include "crud_handler.h"
#include "request_handler.h"
#include "request_handler_factory.h"

class crud_handler_factory : public request_handler_factory {
public:
    crud_handler_factory(const std::string& path, NginxConfig& config);
    request_handler* create(const std::string& location, const std::string& url);

private:
    std::string location_;
    NginxConfig config_;
};


#endif
