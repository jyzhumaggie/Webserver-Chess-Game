#ifndef REPLY_H
#define REPLY_H

#include <string>
#include <vector>
#include <boost/asio.hpp>

#include "header.h"

/** 
 * reply module
 *
 * initializes struct of possible replies with all the status and error codes
 * 
 * stock_replies defines the HTML format for the status/error messages to be 
 * delivered by the server for the the status codes described in the reply struct 
 */

namespace http {
    namespace server {
        namespace misc_strings {
            const char name_value_separator[] = { ':', ' ' };
            const char crlf[] = { '\r', '\n' };
        } 
        struct reply {
            enum status_type
            {
                ok = 200,
                created = 201,
                accepted = 202,
                no_content = 204,
                multiple_choices = 300,
                moved_permanently = 301,
                moved_temporarily = 302,
                not_modified = 304,
                bad_request = 400,
                unauthorized = 401,
                forbidden = 403,
                not_found = 404,
                internal_server_error = 500,
                not_implemented = 501,
                bad_gateway = 502,
                service_unavailable = 503
            } status;
            
            std::vector<header> headers; //headers to be included in reply
            std::string content; //stuff to send in reply
            std::vector<boost::asio::const_buffer> to_buffers(); //function to convert reply into vector of buffers
            static reply stock_reply(status_type status);
        };
        namespace stock_replies {
            std::string to_string(reply::status_type status);
        }
        namespace status_strings {
            boost::asio::const_buffer to_buffer(reply::status_type status);
        }
    }
}

#endif