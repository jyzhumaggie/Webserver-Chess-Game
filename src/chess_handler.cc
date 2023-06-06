#include "chess_handler.h"
#include <boost/algorithm/string/replace.hpp>
#include "chess/board.h"
#include "chess/movegen.h"
#include "chess/defs.h"

#include <fstream>  // for reading the HTML file
#include <iterator>
#include <sstream> 

std::string get_fen(std::string location, std::string request_url){
    if (request_url.size() < location.size()) {
        return "";
    }
    return request_url.substr(location.size());
}

chess_handler::chess_handler(string location, string request_url, NginxConfig& config){
    location_ = location;
    request_url_ = request_url;
    config_ = config;
}

beast::http::status generate_error_res(beast::http::response<beast::http::dynamic_body>& res, beast::http::status s, std::string msg){
    res.result(s);
    beast::ostream(res.body()) << msg;
    res.set(beast::http::field::content_type, "text/plain");
    res.prepare_payload();
    return s;
}

std::string read_html_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        // Return an empty string or an error message
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

beast::http::status chess_handler::serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res){
    std::string fen = get_fen(location_, request_url_);

    //Match endpoints and replace with roots from configuration file
    std::string request_path = request_url_;
    std::vector<path> paths = config_.get_path_from_config();

    for (size_t i = 0; i < paths.size(); i++) {
        std::string search = paths[i].endpoint;
        std::string replace = paths[i].root;

        if(search == location_)
        {
            //handles optional trailing slash for root
            if (replace.back()!='/')
            {
                replace += "/";
            }

            size_t pos = request_path.find(search);
            request_path.replace(pos, search.length(), replace);
            break;
        }
    }

    std::string home_screen = request_path + "index.html";

    if (fen == "") {
        std::string response_content = read_html_file(home_screen);
        if (response_content == "") {
            response_content = "Could not read file";
        }
        res.result(beast::http::status::ok);
        res.version(req.version());
        res.set(beast::http::field::content_type, "text/html");
        beast::ostream(res.body()) << response_content;
        res.prepare_payload();
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

beast::http::status chess_handler::move_pieces(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res){
    //have not decided yet how we're going to get user input for startPos and endPos for each move
    //so these variables are initialized to temp values, will be updated later
    //as an example this is modelling a knight moving from g1 to f3
    int startRow = 7;
    int startCol = 6;
    int endRow = 5;
    int endCol = 5;
    int piece = NOPIECE; //initialized to NOPIECE for testing purposes
    
    Movegen* m;
    bool isPseudoLegal = false;
    std::string fen = "temp fen for testing"; //will be updated in next commit when I handle GET request

    m->getBoard()->parseFen(fen);
    std::list<int> moves = m->generateMoves();
    int userMove = m->getMove(startRow, startCol, endRow, endCol, piece);

    //Ensure the inputted move is at least a pseudolegal move
    for (std::list<int>::iterator itr = moves.begin(); itr != moves.end(); itr++)
    {
        if (*itr == userMove)
        {
            isPseudoLegal = true;
            break;
        }
    }

    bool result;
    if (!isPseudoLegal)
    {
        result = false;
    }
    else
    {
        result = m->makeMove(userMove);
    }

    if(!result){
        generate_error_response(res, beast::http::status::bad_request, "Invalid move, please try again!");
        return res.result();
    }

    // returning status ok in case that it was a legal move
    Board b;
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