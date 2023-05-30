#include "sleep_handler.h"

sleep_handler::sleep_handler(string location, string request_url){

}

beast::http::status sleep_handler::serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res){
    usleep(SLEEP_TIME);
    res.result(boost::beast::http::status::ok);
    std::string request_string = "This request has slept for " + std::to_string(SLEEP_TIME) + " microseconds";
    boost::beast::ostream(res.body()) << request_string;
    
    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "text/plain");
    
    return res.result();
}