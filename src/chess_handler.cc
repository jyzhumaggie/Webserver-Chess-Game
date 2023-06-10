#include "chess_handler.h"
#include <boost/algorithm/string/replace.hpp>
#include "chess/board.h"
#include "chess/movegen.h"
#include "chess/searcher.h"
#include "chess/defs.h"

#include <fstream>  // for reading the HTML file
#include <iterator>
#include <sstream> 

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
    else if (request_url_.find("+ai") != std::string::npos) {
        return handle_ai(req, res);
    }
    else if (request_url_.find("+") != std::string::npos) {
        return move_pieces(req, res);
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
    boost::replace_all(fen, "%20", " ");    //Browser uses %20 to handle spaces 
    int index_of_plus_sign = request_url_.find("+");

    //case that it is /fen+move request and move string is something other than 4 digits
    if(index_of_plus_sign + 5 != request_url_.length())
    {
        generate_error_response(res, beast::http::status::bad_request, "Invalid move, please try again!");
        return res.result();
    }
    else
    {
        int piece = NOPIECE; //initialized to NOPIECE for testing purposes
        int startRow = request_url_[index_of_plus_sign + 1] - '0';
        int startCol = request_url_[index_of_plus_sign + 2] - '0';
        int endRow = request_url_[index_of_plus_sign + 3] - '0';
        int endCol = request_url_[index_of_plus_sign + 4] - '0';

        if (startRow < 0 || startRow > 7 || endRow < 0 || endRow > 7 || startCol < 0 || startCol > 7 || endCol < 0 || endCol > 7) {
            generate_error_response(res, beast::http::status::bad_request, "Invalid move, please try again!");
            return res.result();
        }
    
        Searcher* s = new Searcher();
        Movegen* m = s->getMoveGenerator();
        Board* b = m->getBoard();
        bool isPseudoLegal = false;

        b->parseFen(fen);
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
            delete s;
            return res.result();
        }

        // returning status ok in case that it was a legal move
        std::stringstream buffer;  //redirects the printBoard output from cout to our response body
        std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());
        b->printBoard(b->getSide());
        std::string res_body = buffer.str();
        std::cout.rdbuf(old);      //restores cout to its original stream

        std::string new_fen = b->getFen();
        //concatenating updated fen to the back end of the response body
        //part after + sign will be the updated fen
        res_body = res_body + "+" + new_fen;

        //Check if the game has ended
        //appending w(white wins), b(black wins), d(draw), x(game not yet over) to response body
        int gameResult = m->gameWinner();
        if(gameResult == 0)
        {
            res_body = res_body + "+w";
        }
        else if(gameResult == 1)
        {
            res_body = res_body + "+b";
        }
        else if(gameResult == 2)
        {
            res_body = res_body + "+d";
        }
        else
        {
            res_body = res_body + "+x";
        }

        res.result(boost::beast::http::status::ok);
        boost::beast::ostream(res.body()) << res_body;
    
        res.content_length((res.body().size()));
        res.set(boost::beast::http::field::content_type, "text/plain");
        delete s;
        return res.result();
    }
}

beast::http::status chess_handler::handle_ai(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res) {
    std::string fen = get_fen(location_, request_url_);
    boost::replace_all(fen, "%20", " ");    //Browser uses %20 to handle spaces 
    
    //Now the AI will generate a move
    Searcher* s = new Searcher();
    Movegen* m = s->getMoveGenerator();
    Board* b = m->getBoard();
    b->parseFen(fen);

    int computerMove = s->reccomendMove();
    m->makeMove(computerMove);
    
    // returning status ok in case that it was a legal move
    std::stringstream buffer;  //redirects the printBoard output from cout to our response body
    std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());
    b->printBoard(b->getSide());
    std::string res_body = buffer.str();
    std::cout.rdbuf(old);      //restores cout to its original stream

    std::string new_fen = b->getFen();
    //concatenating updated fen to the back end of the response body
    //part after + sign will be the updated fen
    res_body = res_body + "+" + new_fen;

    //Check if the game has ended
    //appending w(white wins), b(black wins), d(draw), x(game not yet over) to response body
    int gameResult = m->gameWinner();
    if(gameResult == 0)
    {
        res_body = res_body + "+w";
    }
    else if(gameResult == 1)
    {
        res_body = res_body + "+b";
    }
    else if(gameResult == 2)
    {
        res_body = res_body + "+d";
    }
    else
    {
        res_body = res_body + "+x";
    }

    res.result(boost::beast::http::status::ok);
    boost::beast::ostream(res.body()) << res_body;

    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "text/plain");
    delete s;
    return res.result();
}

std::string chess_handler::get_fen(std::string location, std::string request_url){
    if (request_url.size() < location.size()) {
        return "";
    }
    std::string temp = request_url.substr(location.size());
    if (temp.find("+") == std::string::npos)
    {
        return temp;
    }
    else
    {
        return temp.substr(0,temp.find("+"));
    }
}

beast::http::status chess_handler::generate_error_response(beast::http::response<beast::http::dynamic_body>& res, beast::http::status s, std::string msg){
    res.result(s);
    beast::ostream(res.body()) << msg;
    res.set(beast::http::field::content_type, "text/plain");
    res.prepare_payload();
    return s;
}