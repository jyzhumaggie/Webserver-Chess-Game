#include "file_handler.h"
#include "request_handler.h"
#include "reply.h"
#include <iostream>
#include <fstream>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

file_handler::file_handler(std::string location, std::string request_url) {
    
}

beast::http::status file_handler::serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res) {
    // return RequestStatus::Null;

    beast::http::status OK = boost::beast::http::status::ok;
    return OK;
}