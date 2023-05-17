#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#include <string>

// A header structure for sending back HTTP structured replies
namespace http {
    namespace server {
        struct header {
        std::string name;
        std::string value;
        };
    }
}

#endif