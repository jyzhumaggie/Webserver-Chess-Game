#include "echo_handler.h"
#include "request_handler.h"

echo_handler::echo_handler(string location, string request_url){

}

beast::http::status echo_handler::serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res){
    BOOST_LOG_TRIVIAL(info) << "Echo_handler servin! here!!";

    std::string request_string = req.target().to_string();
    res.result(boost::beast::http::status::ok);
    boost::beast::ostream(res.body()) << req;
    
    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "text/plain");
    
    return res.result();
}