#ifndef CRUD_HANDLER_H
#define CRUD_HANDLER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include <map>
#include <set>

#include "config_parser.h"
#include "request_handler.h"


class crud_handler : public request_handler {
public:
    crud_handler(std::string location, std::string request_url, NginxConfig& config, std::map<std::string, std::set<int>>* entities);
    beast::http::status serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res);

private:
    std::string location_;
    std::string request_url_;
    std::string data_path_;
    std::string entity_;
    int id_;
    std::map<std::string, std::set<int>>* entities_;
    NginxConfig config_;
    beast::http::status handle_create(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res);
    beast::http::status handle_retrieve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res);
    beast::http::status handle_update(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res);
    beast::http::status handle_delete(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res);
    beast::http::status handle_list(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res);
};

#endif
