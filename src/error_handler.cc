#include "error_handler.h"


error_handler::error_handler(std::string location, std::string request_url){

}

beast::http::status error_handler::serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res){

    std::string request_string = req.target().to_string();
    res.result(boost::beast::http::status::bad_request);
    
    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "text/plain");
    
    return res.result();
}