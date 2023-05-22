#include "crud_handler.h"

#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>

#include <string>

#include "config_parser.h"
#include "request_handler.h"

namespace http = boost::beast::http;

crud_handler::crud_handler(std::string location, std::string request_url, NginxConfig& config) {
    location_ = location;
    request_url_ = request_url;
    config_ = config;
}

http::status crud_handler::serve(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    switch(req.method()) {
        case http::verb::post:
            return handle_create(req, res);
        case http::verb::get:
            if (req.target().substr(location_.size()).find_first_not_of("/") == std::string::npos) {
                return handle_list(req, res);
            }

            return handle_retrieve(req, res);
        case http::verb::put:
            return handle_update(req, res);
        case http::verb::delete_:
            return handle_delete(req, res);
        default:
            res.result(http::status::method_not_allowed);
            boost::beast::ostream(res.body()) << "Method not allowed";
            res.set(http::field::content_type, "text/plain");
            res.prepare_payload();
            return res.result();
    }
}

http::status crud_handler::handle_create(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    // TODO: not implemented
    BOOST_LOG_TRIVIAL(debug) << "Handling create CRUD\n";
    return http::status::ok;
}

http::status crud_handler::handle_retrieve(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    // TODO: not implemented
    BOOST_LOG_TRIVIAL(debug) << "Handling retrieve CRUD\n";
    return http::status::ok;
}

http::status crud_handler::handle_update(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    // TODO: not implemented
    BOOST_LOG_TRIVIAL(debug) << "Handling update CRUD\n";
    return http::status::ok;
}

http::status crud_handler::handle_delete(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    // TODO: not implemented
    BOOST_LOG_TRIVIAL(debug) << "Handling delete CRUD\n";
    return http::status::ok;
}

http::status crud_handler::handle_list(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    // TODO: not implemented
    BOOST_LOG_TRIVIAL(debug) << "Handling list CRUD\n";
    return http::status::ok;
}
