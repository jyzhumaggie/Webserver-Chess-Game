#include "request_handler.h"

request_handler::request_handler(std::string base_dir) {
    path = base_dir;
}

bool request_handler::parse(std::string request) {
    std::vector<std::string> lines = get_lines(request);
    if (lines.size() == 0) {
        return false;
    }
    std::vector<std::string> request_line_tokens = get_tokens_from_request_line(lines[0]);
    if (request_line_tokens.size() != 3) {
        return false;
    }
    method = request_line_tokens[0];
    path = request_line_tokens[1];
    http_type = request_line_tokens[2];

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