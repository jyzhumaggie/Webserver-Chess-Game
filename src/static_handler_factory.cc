#include "static_handler_factory.h"
#include <cstddef>

// Note: delete after updating static_handler
#include "config_parser.h"

static_handler_factory::static_handler_factory(const std::string& path, NginxConfig& config) {
    location_ = path;
    config_ = config;
}

request_handler* static_handler_factory::create(const std::string& location, const std::string& url)
{

    //TODO: will need to be later changed to new static_handler(const std::string& location,NginxConfig& config)
    return new static_handler(location, url, config_);
}