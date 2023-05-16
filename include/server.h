#ifndef SERVER_H
#define SERVER_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "config_parser.h"
// #include <../src/session.cc>
#include "echo_handler_factory.h"
#include "static_handler_factory.h"
#include "request_handler_factory.h"

using boost::asio::ip::tcp;

class server
{
    public:
        server(boost::asio::io_service& io_service, short port, std::vector<path> paths, NginxConfig& config, std::map<std::string, std::string> handler_names);

        void create_handler_factory(const std::string& name, NginxConfig& config, const std::string& endpoint);
    private:
        void start_accept();

        void handle_accept(session* new_session,
            const boost::system::error_code& error);

        boost::asio::io_service& io_service_;
        tcp::acceptor acceptor_;
        std::vector<path> paths_;
        echo_handler_factory* echo_handler_factory_;
        static_handler_factory* static_handler_factory_;
        std::map<std::string, std::string> handler_names_;
        std::map<std::string, request_handler_factory*> routes_;
        NginxConfig config_;
};


#endif