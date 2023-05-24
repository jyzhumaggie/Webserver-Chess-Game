#include "crud_handler.h"

#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp> // https://json.nlohmann.me/

#include "config_parser.h"
#include "request_handler.h"
#include "file_system.h"

namespace http = boost::beast::http;
using json = nlohmann::json;

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
        if(id_str == "") return -1;
        id = std::stoi(id_str);
        return (id > 0)? id : -1;
    }
}

std::string get_data_path(NginxConfig& config, std::string location){
    std::vector<path> paths = config.get_path_from_config();
    for (size_t i = 0; i < paths.size(); i++){
        if(location == paths[i].endpoint){
            return paths[i].root;
        }
    }
    return "";
}

http::status generate_error_response(http::response<http::dynamic_body>& res, http::status s, std::string msg){
    res.result(s);
    boost::beast::ostream(res.body()) << msg;
    res.set(http::field::content_type, "text/plain");
    res.prepare_payload();
    return s;
}

crud_handler::crud_handler(std::string location, std::string request_url, NginxConfig& config, std::map<std::string, std::set<int>>* entities, file_system* fs) {
    location_ = location;
    request_url_ = request_url;
    config_ = config;
    entities_= entities;
    filesystem_ = fs;

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
        return generate_error_response(res, http::status::bad_request, "Invalid entity");
    }
    if(data_path_ == ""){
        return generate_error_response(res, http::status::bad_request, "Invalid data path");
    }
    if(id_ == -1){
        return generate_error_response(res, http::status::bad_request, "Invalid id");
    }

    switch(req.method()) {
        case http::verb::post:
            return handle_create(req, res);
        case http::verb::get:
            if (id_ == 0) {
                return handle_list(req, res);
            }
            return handle_retrieve(req, res);
        case http::verb::put:
            return handle_update(req, res);
        case http::verb::delete_:
            return handle_delete(req, res);
        default:
            return generate_error_response(res, http::status::method_not_allowed, "Method not allowed");
    }
}

http::status crud_handler::handle_create(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    BOOST_LOG_TRIVIAL(debug) << "Handling create CRUD\n";

    std::string req_body = boost::beast::buffers_to_string(req.body().data());

    // validate JSON request body
    json j;
    try{
        j = json::parse(req_body);
    }
    catch (json::parse_error& ex){
        BOOST_LOG_TRIVIAL(error) << "JSON error: request body is not a valid JSON file\n";
        return generate_error_response(res, http::status::bad_request, "Invalid JSON request body");
    }

    // create a new entity if needed
    if(entities_->find(entity_) == entities_->end()){
        entities_->insert({entity_, std::set<int>{}});
        filesystem_->create_directory(data_path_ + entity_);
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
        BOOST_LOG_TRIVIAL(error) << "Id specified in CRUD create, no file is created\n";
        return generate_error_response(res, http::status::bad_request, "Id specified in CRUD create");
    }
    entities_->find(entity_)->second.insert(id_);
    filesystem_->write_file(data_path_ + entity_ + "/" + std::to_string(id_), j);

    // create JSON response
    std::string res_body = "{\"id\": " + std::to_string(id_) + "}";
    boost::beast::ostream(res.body()) << res_body;
    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "application/json");
    res.result(http::status::created);
    return http::status::created;
}

http::status crud_handler::handle_retrieve(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    BOOST_LOG_TRIVIAL(debug) << "Handling retrieve CRUD\n";
    if (entities_->find(entity_) == entities_->end()) {
        BOOST_LOG_TRIVIAL(error) << "Entity " << entity_ << " does not exist\n";
        return generate_error_response(res, http::status::not_found, "Entity does not exist");
    }

    if(entities_->find(entity_)->second.find(id_) == entities_->find(entity_)->second.end()){
        BOOST_LOG_TRIVIAL(error) << "Invalid id specified in CRUD retrieve\n";
        return generate_error_response(res, http::status::not_found, "Invalid id specified in CRUD retrieve");
    }

    std::string file_path = data_path_ + entity_ + "/" + std::to_string(id_);

    // read data from file
    nlohmann::json data;
    filesystem_->read_file(file_path, data);

    // create response
    boost::beast::ostream(res.body()) << data;
    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "application/json");
    res.result(http::status::ok);
    return http::status::ok;
}

http::status crud_handler::handle_update(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) { 
    BOOST_LOG_TRIVIAL(debug) << "Handling update CRUD\n";

    if(id_ == 0){
        return generate_error_response(res, http::status::bad_request, "No id specified in CRUD update");
    }

    if(entities_->find(entity_) == entities_->end()){
        return generate_error_response(res, http::status::not_found, "No matched entity found");
    }

    // check the content to update, if it is not a JSON file, do not update
    std::string req_body = boost::beast::buffers_to_string(req.body().data());

    json j;
    try{
        j = json::parse(req_body);
    }
    catch (json::parse_error& ex){
        BOOST_LOG_TRIVIAL(error) << "JSON error: request body is not a valid JSON file\n";
        return generate_error_response(res, http::status::bad_request, "Invalid JSON request body");
    }

    // if the id exists, update the file
    std::string file_path = data_path_ + entity_ + "/" + std::to_string(id_);
    bool id_exists = false;
    if(entities_->find(entity_)->second.find(id_) != entities_->find(entity_)->second.end()){
        id_exists = true;
    }
    else{ // otherwise, create a new file and insert the id to set
        entities_->find(entity_)->second.insert(id_);
    }
    filesystem_->write_file(file_path, j);

    std::string res_body = "{\"id\": " + std::to_string(id_) + "}";
    boost::beast::ostream(res.body()) << res_body;
    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "application/json");
    http::status s = (id_exists ? http::status::ok : http::status::created);
    res.result(s);
    return s;
}

http::status crud_handler::handle_delete(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    BOOST_LOG_TRIVIAL(debug) << "Handling delete CRUD\n";

    if(entities_->find(entity_) == entities_->end()){
        return generate_error_response(res, http::status::bad_request, "Invalid entity specified in CRUD delete");
    }
    if(entities_->find(entity_)->second.find(id_) == entities_->find(entity_)->second.end()){
        return generate_error_response(res, http::status::not_found, "Invalid id specified in CRUD delete");
    }

    // remove file
    std::string file_path = data_path_ + entity_ + "/" + std::to_string(id_);
    filesystem_->delete_file(file_path);

    // release the id from the set
    entities_->find(entity_)->second.erase(id_);

    boost::beast::ostream(res.body()) << "Deleted " << id_;
    res.result(http::status::ok);
    res.prepare_payload();
    return http::status::ok;
}

http::status crud_handler::handle_list(const http::request<http::dynamic_body> req, http::response<http::dynamic_body>& res) {
    BOOST_LOG_TRIVIAL(debug) << "Handling list CRUD\n";
   
    std::string list_of_ids;

    if(entities_->find(entity_) != entities_->end()){
        // iterate the set of ids
        for(auto itr : entities_->find(entity_)->second){
            list_of_ids = list_of_ids + ", " + std::to_string(itr);
        }
        list_of_ids = list_of_ids.substr(2);
    }

    list_of_ids = "[" + list_of_ids + "]";
    boost::beast::ostream(res.body()) << list_of_ids;
    res.content_length((res.body().size()));
    res.set(boost::beast::http::field::content_type, "application/json");
    res.result(http::status::ok);
    return http::status::ok;
}
