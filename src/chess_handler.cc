#include "chess_handler.h"
#include <boost/algorithm/string/replace.hpp>
#include "chess/board.h"

chess_handler::chess_handler(string location, string request_url){
    location_ = location;
    request_url_ = request_url;
}

beast::http::status chess_handler::serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res){
    std::string fen = get_fen(location_, request_url_);
    if (fen == "") {
        generate_error_response(res, beast::http::status::bad_request, "Invalid location, request_url pair");
        return res.result();
    }
    
    boost::replace_all(fen, "%20", " ");    //Browser uses %20 to handle spaces
    Board b;
    if (!b.parseFen(fen)) {
        generate_error_response(res, beast::http::status::bad_request, "Invalid fen");
        return res.result();
    }

    std::stringstream buffer;  //redirects the printBoard output from cout to our response body
    std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());
    b.printBoard(b.getSide());
    std::string res_body = buffer.str();
    std::cout.rdbuf(old);      //restores cout to its original stream

    res.result(boost::beast::http::status::ok);
    boost::beast::ostream(res.body()) << res_body;
    
    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "text/plain");
    return res.result();
}

std::string chess_handler::get_fen(std::string location, std::string request_url){
    if (request_url.size() < location.size()) {
        return "";
    }
    return request_url.substr(location.size());
}

beast::http::status chess_handler::generate_error_response(beast::http::response<beast::http::dynamic_body>& res, beast::http::status s, std::string msg){
    res.result(s);
    beast::ostream(res.body()) << msg;
    res.set(beast::http::field::content_type, "text/plain");
    res.prepare_payload();
    return s;
}