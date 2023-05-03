#include "echo_handler.h"
#include "request_handler.h"
#include "reply.h"

echo_handler::echo_handler(std::string base_dir) : request_handler(base_dir) {
}

reply echo_handler::handle_request() {
    reply reply_;
    reply_.status = reply::bad_request;
    reply_.content = complete_request;
    reply_.headers.resize(2);
    reply_.headers[0].name = "Content-Length";
    reply_.headers[0].value = std::to_string(complete_request.size());
    reply_.headers[1].name = "Content-Type";
    reply_.headers[1].value = "text/plain"; //content type is text/plain

    return reply_;
}