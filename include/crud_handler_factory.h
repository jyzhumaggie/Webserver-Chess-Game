#ifndef CRUD_HANDLER_FACTORY_H
#define CRUD_HANDLER_FACTORY_H

#include <string>
#include <map>
#include <set>

#include "config_parser.h"
#include "crud_handler.h"
#include "request_handler.h"
#include "request_handler_factory.h"

class file_system;
class crud_handler_factory : public request_handler_factory {
public:
    crud_handler_factory(const std::string& path, NginxConfig& config, std::map<std::string, std::set<int>>* entities, file_system* fs);
    request_handler* create(const std::string& location, const std::string& url);

private:
    file_system* filesystem_;
    std::string location_;
    NginxConfig config_;
    std::map<std::string, std::set<int>>* entities_;
};


#endif
