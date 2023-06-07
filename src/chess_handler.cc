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
    std::string fen = get_fen(location_, request_url_);
    std::string request_body = boost::beast::buffers_to_string(req.body().data());    
    int index_of_plus_sign = request_body.find("+");

    if(index_of_plus_sign+5 > request_body.length())
    {
        generate_error_response(res, beast::http::status::bad_request, "Invalid move, please try again!");
        return res.result();
    }

    int startRow = stoi(request_body.substr(index_of_plus_sign+1, index_of_plus_sign+2));
    int startCol = stoi(request_body.substr(index_of_plus_sign+2, index_of_plus_sign+3));
    int endRow = stoi(request_body.substr(index_of_plus_sign+3, index_of_plus_sign+4));
    int endCol = stoi(request_body.substr(index_of_plus_sign+4, index_of_plus_sign+5));

    if(startRow<0 || startRow>7 || startCol<0 || startCol>7 || endRow<0 || endRow>7 || endCol<0 || endCol>7)
    {
        generate_error_response(res, beast::http::status::bad_request, "Invalid move, please try again!");
        return res.result();
    }

    else
    {
        int piece = NOPIECE; //initialized to NOPIECE for testing purposes
    
        Movegen* m;
        bool isPseudoLegal = false;

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
            //given that move is pseudolegal, testing if it is legal using makeMove function
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

        std::string new_fen = b.getFen();
        //concatenating updated fen to the back end of the response body
        //part after + sign will be the updated fen
        res_body = res_body + "+" + new_fen;

        res.result(boost::beast::http::status::ok);
        boost::beast::ostream(res.body()) << res_body;
    
        res.content_length((res.body().size()));
        res.set(boost::beast::http::field::content_type, "text/plain");
        return res.result();
    }
}

std::string chess_handler::get_fen(std::string location, std::string request_url){
    if (request_url.size() < location.size()) {
        return "";
    }
    std::string temp = request_url.substr(location.size());
    //this logic will get just the fen and remove everything that comes after + sign which denotes the move
    int index = temp.find("+");
    //checks for the case the only /chess/FEN is passed
    //because we don't want to incorrectly throw an error if the +move part isn't present in the string
    if (index == -1)
    {
        return temp;
    }
    else
    {
        return temp.substr(0,index);
    }
}

beast::http::status chess_handler::generate_error_response(beast::http::response<beast::http::dynamic_body>& res, beast::http::status s, std::string msg){
    res.result(s);
    beast::ostream(res.body()) << msg;
    res.set(beast::http::field::content_type, "text/plain");
    res.prepare_payload();
    return s;
}