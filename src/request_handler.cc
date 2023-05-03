#include "request_handler.h"
#include <iostream>
#include <boost/log/trivial.hpp>

request_handler::request_handler(std::string base_dir,std::vector<path> paths) {
    root_path = base_dir;
    paths_ = paths;
}

bool request_handler::parse(std::string request) {
    complete_request = request;
    std::vector<std::string> lines = get_lines(request);
    if (lines.size() == 0) {
        return false;
    }
    std::vector<std::string> request_line_tokens = get_tokens_from_request_line(lines[0]);
    if (request_line_tokens.size() != 3) {
        return false;
    }
    method = request_line_tokens[0];
    if (method != "GET") { //this needs to be expanded if we implement more methods
        return false;
    }

    std::string requestPath = request_line_tokens[1];

    //Match endpoints and replace with roots from configuration file
    for (size_t i = 0; i < paths_.size(); i++) {
        std::string search = paths_[i].endpoint;
        std::string replace = paths_[i].root;

        size_t pos = requestPath.find(search);

        if (pos != std::string::npos) {
            requestPath.replace(pos, search.length(), replace);
            break;
        }
    }

    root_path+=requestPath;
    
    http_type = request_line_tokens[2];
    if (http_type != "HTTP/1.1") {
        return false;
    }
    filename = get_filename(root_path);
    if (filename == "") {
        return false;
    }
    extension = get_extension(filename);

    for (int i = 1; i < lines.size(); i++) {
        if (valid_header(lines[i])) {
            headers.push_back(get_header(lines[i]));
        }
        else {
            return false;
        }
    }
    return true;
}

std::vector<std::string> request_handler::get_lines(std::string input) {
    std::vector<std::string> lines;
    std::string temp_string = "";
    for (int i = 0; i < input.size(); i++) {
        if (input[i] == '\n' && temp_string != "") {
            lines.push_back(temp_string);
            temp_string = "";
        }
        //Ignoring carriage returns so they don't get put into the temp_string
        else if (input[i] != '\r') {
            temp_string += input[i];
        }
    }
    return lines;
}

std::vector<std::string> request_handler::get_tokens_from_request_line(std::string request_line) {
    std::vector<std::string> tokens;
    std::string temp_token = "";
    for (int i = 0; i < request_line.size(); i++) {
        if (request_line[i] != ' ') {
            temp_token += request_line[i];
        }
        else {
            if (temp_token != "") {
                tokens.push_back(temp_token);
                temp_token = "";
            }
        }
    }
    if (temp_token != "") {
        tokens.push_back(temp_token);
    }
    return tokens;
}

bool request_handler::valid_header(std::string header) {
    int separators = 0;
    for (int i = 0; i < header.size(); i++) {
        if (header[i] == ':') {
            separators++;
        }
    }
    return (separators == 1);
}

header request_handler::get_header(std::string header) {
    std::string header_name = "";
    std::string header_value = "";
    bool separator_passed = false;
    for (int i = 0; i < header.size(); i++) {
        if (!separator_passed) {
            if (header[i] != ' ') {
                header_name += header[i];
            }
            else if (header[i] == ':') {
                separator_passed = true;
            }
        }
        else{
           if (header[i] != ' ') {
                header_value += header[i];
           }
        }
    }
    http::server::header header_t;
    header_t.name = header_name;
    header_t.value = header_value;
    return header_t;
}

std::string request_handler::get_filename(std::string root_path) {
    int last_slash = 0;
    for (int i = 0; i < root_path.size(); i++) {
        if (root_path[i] == '/') {
            last_slash = i;
        }
    }
    if (last_slash + 1 == root_path.size()) {
        return "";
    }
    return root_path.substr(last_slash + 1, root_path.size() - last_slash - 1);
}

std::string request_handler::get_extension(std::string filename) {
    int first_period = filename.size();
    for (int i = 0; i < filename.size(); i++) {
        if (filename[i] == '.') {
            first_period = i;
            break;
        }
    }
    if (first_period == filename.size()) {
        return "";
    }
    return filename.substr(first_period + 1, filename.size() - first_period - 1);
}
