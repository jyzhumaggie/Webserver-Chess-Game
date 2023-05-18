#include "not_found_handler.h"
#include "request_handler.h"
#include <boost/log/trivial.hpp>


not_found_handler::not_found_handler(std::string location, std::string request_url){

}

beast::http::status not_found_handler::serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res){

    std::string request_string = req.target().to_string();
    res.result(boost::beast::http::status::not_found);
    // boost::beast::ostream(res.body()) << req;
    
    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "text/plain");
    
    return res.result();
}