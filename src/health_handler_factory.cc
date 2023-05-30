#include "health_handler_factory.h"

health_handler_factory::health_handler_factory(const std::string& path, NginxConfig& config) {
    location_ = path;
    config_ = config;
}

request_handler* health_handler_factory::create(const std::string& location, const std::string& url)
{
    return new health_handler(location_, url);
}
