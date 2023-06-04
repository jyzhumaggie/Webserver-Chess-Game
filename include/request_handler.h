#ifndef REQUEST_HANLDER_H
#define REQUEST_HANLDER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>
#include "config_parser.h"

namespace beast = boost::beast;

class request_handler {
    public:
        virtual beast::http::status serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res) = 0;
};


#endif