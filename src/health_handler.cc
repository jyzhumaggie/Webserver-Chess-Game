#include "health_handler.h"

health_handler::health_handler(string location, string request_url){

}

beast::http::status health_handler::serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res){
    res.result(boost::beast::http::status::ok);
    std::string res_body = "OK";
    boost::beast::ostream(res.body()) << res_body;
    
    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "text/plain");
    return res.result();
}