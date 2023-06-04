#include "chess_handler_factory.h"

chess_handler_factory::chess_handler_factory(const std::string& path, NginxConfig& config) {
    location_ = path;
    config_ = config;
}

request_handler* chess_handler_factory::create(const std::string& location, const std::string& url)
{
    return new chess_handler(location_, url, config_);
}
