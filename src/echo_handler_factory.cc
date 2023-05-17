#include "echo_handler_factory.h"

echo_handler_factory::echo_handler_factory(const std::string& path, NginxConfig& config) {
    location_ = path;
    config_ = config;
}

request_handler* echo_handler_factory::create(const std::string& location, const std::string& url)
{
    //TODO: will need to be later changed to new echo_handler(const std::string& location,NginxConfig& config)
    return new echo_handler(location_, url);
}
