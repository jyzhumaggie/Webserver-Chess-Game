#include "sleep_handler_factory.h"

sleep_handler_factory::sleep_handler_factory(const std::string& path, NginxConfig& config) {
    location_ = path;
    config_ = config;
}

request_handler* sleep_handler_factory::create(const std::string& location, const std::string& url)
{
    return new sleep_handler(location_, url);
}
