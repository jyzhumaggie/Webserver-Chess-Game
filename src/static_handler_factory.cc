#include "static_handler_factory.h"
#include <cstddef>

// Note: delete after updating static_handler
#include "config_parser.h"

static_handler_factory::static_handler_factory(const std::string& path, NginxConfig& config) {
    
}

request_handler* static_handler_factory::create(const std::string& location, NginxConfig& config)
{
    //TODO: will need to be later changed to new static_handler(const std::string& location,NginxConfig& config)
    std::vector<path> temp_paths_to_be_deleted;
    return new file_handler("",temp_paths_to_be_deleted);
}