#include "crud_handler.h"

#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "config_parser.h"
#include "request_handler.h"

namespace http = boost::beast::http;

std::string get_entity(std::string location, std::string request_url){
    std::size_t entity_pos = request_url.find(location);
    if(entity_pos == 0){
        entity_pos += location.size();
        std::string entity = request_url.substr(entity_pos);
        std::size_t slash_pos = entity.find('/');
        if(slash_pos != std::string::npos){
            entity = entity.substr(0, slash_pos);
        }
        return entity;
    }
    else return "";
}

int get_id(std::string request_url, std::string entity, std::string location){
    // return 0: no id specified
    // return -1: invalid id specified (id < 1)
    int id = 0;

    std::string location_entity = location + entity;

    if(request_url == location_entity){
        return 0;
    }
    else{
        std::string id_str = request_url.substr(location_entity.size()+1);
        id = std::stoi(id_str);
        return (id > 0)? id : -1;
    }
}

std::string get_data_path(NginxConfig& config, std::string location){
    std::<path> paths = config.get_path_from_config();
    for (size_t i = 0; i < paths.size(); i++){
        if(location == paths[i].endpoint){
            return paths[i].root;
        }
    }
    return "";
}

void generate_error_response(http::response<http::dynamic_body>& res, http::status s, std::string msg){
    res.result(s);
    boost::beast::ostream(res.body()) << msg;
    res.set(http::field::content_type, "text/plain");
    res.prepare_payload();
}

crud_handler::crud_handler(std::string location, std::string request_url, NginxConfig& config, std::map<std::string, std::set<int>>* entities) {
    
    location_ = location;
    request_url_ = request_url;
    config_ = config;
    entities_= entities;

    entity_ = get_entity(location, request_url);
    if(entity_ == ""){
        BOOST_LOG_TRIVIAL(error) << "Invalid entity name specified\n";
        id_ = -1;
    }
    else{
        id_ = get_id(request_url, entity_, location);
    }

    if(id_ == -1){
        BOOST_LOG_TRIVIAL(error) << "Invalid id specified (id < 1)\n";
    }

    data_path_ = get_data_path(config, location);
    if(data_path_ == ""){
        BOOST_LOG_TRIVIAL(error) << "No matched CRUD data_path\n";
    }
}

http::status crud_handler::serve(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    if(entity_ == ""){
        generate_error_response(res, http::status::bad_request, "Invalid entity");
        return http::status::bad_request;
    }
    if(data_path_ == ""){
        generate_error_response(res, http::status::bad_request, "Invalid data path");
        return http::status::bad_request;
    }
    if(id_ == -1){
        generate_error_response(res, http::status::bad_request, "Invalid id");
        return http::status::bad_request;
    }

    switch(req.method()) {
        case http::verb::post:
            return handle_create(req, res);
        case http::verb::get:
            if (req.target().substr(location_.size()).find_first_not_of("/") == std::string::npos) {
                return handle_list(req, res);
            }

            return handle_retrieve(req, res);
        case http::verb::put:
            return handle_update(req, res);
        case http::verb::delete_:
            return handle_delete(req, res);
        default:
            generate_error_response(res, http::status::method_not_allowed, "Method not allowed");
            return res.result();
    }
}

http::status crud_handler::handle_create(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    BOOST_LOG_TRIVIAL(debug) << "Handling create CRUD\n";

    if(entities_->find(entity_) == entities_->end()){
        entities_->insert({entity_, std::set<int>{}});
        std::filesystem::create_directory(data_path_ + entity_);
    }
    
    // determine id and insert into the set
    if(id_ == 0){
        int smallest_available_id = 1;
        // iterate the set of ids
        for(auto itr : entities_->find(entity_)->second){
            if(itr == smallest_available_id){
                smallest_available_id++;
            }
            else break;
        }
        id_ = smallest_available_id;
    }
    else{
        if(entities_->find(entity_)->second.find(id_) != entities_->find(entity_)->second.end()){
            BOOST_LOG_TRIVIAL(error) << "Repeated id specified in CRUD create, no file is created\n";
            generate_error_response(res, http::status::bad_request, "Repeated id specified in CRUD create");
            return http::status::bad_request;
        }
    }
    entities_->find(entity_)->second.insert(id_);
        
    // write data to file
    std::string file_path = data_path_ + entity_ + "/" + std::to_string(id_);
    std::ofstream File(file_path);
    File << boost::beast::buffers_to_string(req.body().data());
    File.close();

    // create response
    boost::beast::ostream(res.body()) << std::to_string(id_);
    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "text/plain");

    return http::status::ok;
}

http::status crud_handler::handle_retrieve(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    BOOST_LOG_TRIVIAL(debug) << "Handling retrieve CRUD\n";

    if(entities_->find(entity_)->second.find(id_) == entities_->find(entity_)->second.end()){
        BOOST_LOG_TRIVIAL(error) << "Invalid id specified in CRUD retrieve\n";
        generate_error_response(res, http::status::not_found, "Invalid id specified in CRUD retrieve");
        return http::status::not_found;
    }

    std::string file_path = data_path_ + entity_ + "/" + std::to_string(id_);

    // read data from file
    std::ifstream File(file_path);
    std::string data;
    File >> data;
    File.close();

    // create response
    boost::beast::ostream(res.body()) << data;
    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "text/plain");

    return http::status::ok;
}

http::status crud_handler::handle_update(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    // TODO: not implemented
    BOOST_LOG_TRIVIAL(debug) << "Handling update CRUD\n";
    return http::status::ok;
}

http::status crud_handler::handle_delete(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    // TODO: not implemented
    BOOST_LOG_TRIVIAL(debug) << "Handling delete CRUD\n";
    return http::status::ok;
}

http::status crud_handler::handle_list(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    // TODO: not implemented
    BOOST_LOG_TRIVIAL(debug) << "Handling list CRUD\n";
    return http::status::ok;
}
