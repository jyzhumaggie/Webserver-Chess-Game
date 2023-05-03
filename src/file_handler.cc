#include "file_handler.h"
#include "request_handler.h"
#include "reply.h"
#include <iostream>
#include <fstream>
#include <string>

file_handler::file_handler(std::string base_dir) : request_handler(base_dir) {
}

reply file_handler::handle_request() {
    reply reply_;

    std::ifstream file(path);
    if (file.is_open() ) { //does file exist?
        file.seekg (0, file.end);
        int length = file.tellg();
        file.seekg (0, file.beg);

        //only works for text files atm!
        char buffer[length + 1];
        file.read(buffer, length);
        buffer[length] = '\0';
        std::string contents = std::string(buffer);

        reply_.status = reply::ok;
        reply_.content = contents;
        reply_.headers.resize(2);
        reply_.headers[0].name = "Content-Length";
        reply_.headers[0].value = std::to_string(length);
        reply_.headers[1].name = "Content-Type";
        reply_.headers[1].value = "text/plain";
    }
    else {
        std::string not_found_html = "<html><body><h1>404 NOT FOUND</h1></body></html>";
        reply_.status = reply::not_found;
        reply_.content = not_found_html;
        reply_.headers.resize(2);
        reply_.headers[0].name = "Content-Length";
        reply_.headers[0].value = std::to_string(not_found_html.size());
        reply_.headers[1].name = "Content-Type";
        reply_.headers[1].value = "text/html";
    }
    return reply_;
}