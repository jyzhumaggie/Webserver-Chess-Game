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
    return new static_handler(location, url, config_);
}