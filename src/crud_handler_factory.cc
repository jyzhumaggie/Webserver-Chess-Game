#include "crud_handler_factory.h"

#include <string>

#include "config_parser.h"
#include "crud_handler.h"

crud_handler_factory::crud_handler_factory(const std::string& path, NginxConfig& config, std::map<std::string, std::set<int>>* entities) {
    location_ = path;
    config_ = config;
    entities_ = entities;
}

request_handler* crud_handler_factory::create(const std::string& location, const std::string& url) {
    return new crud_handler(location, url, config_, entities_);
}
