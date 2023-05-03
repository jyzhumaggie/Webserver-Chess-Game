#include "file_handler.h"
#include "request_handler.h"
#include "reply.h"
#include <iostream>
#include <fstream>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

file_handler::file_handler(std::string base_dir) : request_handler(base_dir) {
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

void file_handler::handle_request(tcp::socket& socket) {
    reply reply_;

    FILE* fp;
    if ((fp = fopen((path.c_str()), "rb"))) {
        int buf_len = 100000;
        char sendbuf[buf_len];  //payload buffer

        //get size of requested file
        int size;
        fseek(fp, 0 , SEEK_END);
        size = ftell(fp);
        rewind(fp);

        reply_.status = reply::ok;
        reply_.content = "";
        reply_.headers.resize(2);
        reply_.headers[0].name = "Content-Length";
        reply_.headers[0].value = std::to_string(size);
        reply_.headers[1].name = "Content-Type";
        //here we test
        if (ext_to_mime.find(extension) != ext_to_mime.end()) {
            reply_.headers[1].value = ext_to_mime.find(extension)->second;
        }
        else {
            reply_.headers[1].value = "text/plain";
        }
        //sends header
        boost::asio::write(socket, reply_.to_buffers());

        //Loads payload into the send buffer and sends off
        int bytes_loaded = 0;
        for (int i = 0; i < size; i++)
        {
            bytes_loaded++;
            sendbuf[i % sizeof(sendbuf)] = fgetc(fp);
            if ((i % sizeof(sendbuf)) == sizeof(sendbuf) - 1)
            {
                boost::asio::write(socket, boost::asio::buffer(sendbuf, bytes_loaded));
                std::cout << "sent" << std::endl;
                bytes_loaded = 0;
                memset(sendbuf, 0, sizeof(sendbuf));
            }
        }
        //Send final chunk
        boost::asio::write(socket, boost::asio::buffer(sendbuf, bytes_loaded));
        fclose(fp);
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
        boost::asio::write(socket, reply_.to_buffers());
    }
}