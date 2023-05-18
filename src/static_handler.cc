#include "static_handler.h"
#include "request_handler.h"
#include "reply.h"
#include <iostream>
#include <fstream>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

static_handler::static_handler(std::string location, std::string request_url, NginxConfig& config) {
    location_ = location;
    request_url_ = request_url;
    config_ = config;
    ext_to_mime["bin"] = "application/octet-stream";
    ext_to_mime["css"] = "text/css";
    ext_to_mime["csv"] = "text/csv";
    ext_to_mime["doc"] = "application/msword";
    ext_to_mime["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    ext_to_mime["gz"] = "application/gzip";
    ext_to_mime["gif"] = "image/gif";
    ext_to_mime["htm"] = "text/html";
    ext_to_mime["html"] = "text/html";
    ext_to_mime["jar"] = "application/java-archive";
    ext_to_mime["jpeg"] = "image/jpeg";
    ext_to_mime["jpg"] = "image/jpeg";
    ext_to_mime["js"] = "text/javascript";
    ext_to_mime["json"] = "application/json";
    ext_to_mime["mp3"] = "audio/mpeg";
    ext_to_mime["mp4"] = "video/mp4";
    ext_to_mime["png"] = "image/png";
    ext_to_mime["pdf"] = "application/pdf";
    ext_to_mime["rar"] = "application/vnd.rar";
    ext_to_mime["rtf"] = "application/rtf";
    ext_to_mime["sh"] = "application/x-sh";
    ext_to_mime["tar"] = "application/x-tar";
    ext_to_mime["txt"] = "text/plain";
    ext_to_mime["wav"] = "audio/wav";
    ext_to_mime["xml"] = "application/xml";
    ext_to_mime["zip"] = "application/zip";
}

std::string static_handler::get_reply(int err_code)
{
    std::string return_string;
    switch (err_code)
    {
        case 200:
          return_string = "";
          break;
        case 400:
          return_string = "<html>"
                          "<head><title>Bad Request</title></head>"
                          "<body><h1>400 Bad Request</h1></body>"
                          "</html>";
          break;
        default:
          return_string = "<html>"
                          "<head><title>Not Found</title></head>"
                          "<body><h1>404 Not Found</h1></body>"
                          "</html>";
          break;
    }
    return return_string;
}

beast::http::status static_handler::serve(const beast::http::request<beast::http::dynamic_body> req, beast::http::response<beast::http::dynamic_body>& res) {
    
    std::string file_name = request_url_.substr(location_.size(), std::string::npos);

    // Determine extension type from substring following final period.
    size_t ext_start = file_name.find_last_of(".");
    std::string extension = "";
    if (ext_start != std::string::npos)
    {
        extension = file_name.substr(ext_start + 1);
    }
    extension = ext_to_mime[extension];


    //Match endpoints and replace with roots from configuration file
    std::string request_path = request_url_;
    std::vector<path> paths = config_.get_path_from_config();

    for (size_t i = 0; i < paths.size(); i++) {
        std::string search = paths[i].endpoint;
        std::string replace = paths[i].root;

        if(search == location_)
        {
            size_t pos = request_path.find(search);
            request_path.replace(pos, search.length(), replace);
            break;
        }
    }
    
    FILE* fp;
    if ((fp = fopen(request_path.c_str(), "rb")))
    {
        
        std::string reply_body = "";
        char c;
        int ch;
        while ((ch = fgetc(fp)) != EOF)
        {
            c = static_cast<char>(ch);
            reply_body += c;
        }

        res.result(boost::beast::http::status::ok);
        boost::beast::ostream(res.body()) << reply_body;
        res.content_length((res.body().size()));
        res.set(boost::beast::http::field::content_type, extension);
        
        fclose(fp);
        
        return res.result();
    }
    else
    {
        res.result(boost::beast::http::status::not_found);
        boost::beast::ostream(res.body()) << get_reply(res.result_int());
        res.content_length((res.body().size()));
        res.set(boost::beast::http::field::content_type, "text/plain");
        return res.result();
    }

}