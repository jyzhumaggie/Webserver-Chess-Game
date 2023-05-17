#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"
#include <boost/regex.hpp>
#include <sstream>
#include "reply.h"
#include "echo_handler.h"
#include "file_handler.h"
#include "error_handler.h"

#include <string>

using boost::asio::ip::tcp;
using namespace std;

session::session(boost::asio::io_service& io_service, NginxConfig& config, std::map<std::string, request_handler_factory*> routes)
		: socket_(io_service)
{
	config_ = config;
}

tcp::socket& session::socket()
{
	return socket_;
}

bool session::set_routes(std::map<std::string, request_handler_factory*> routes){
	routes_ = routes;
}

bool session::start(std::vector<path> paths)
{
    paths_ = paths;
    boost::system::error_code error;
    boost::asio::ip::tcp::endpoint remote_ep = socket_.remote_endpoint(error);
    if(error)
    {
        clientIP_ = "127.0.0.1";
    }
    else
    {
        boost::asio::ip::address remote_ad = remote_ep.address();
        clientIP_ = remote_ad.to_string();
    }
	
	boost::beast::http::async_read(socket_,
		buffer_,
		request_,
		boost::bind(&session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

    BOOST_LOG_TRIVIAL(info) << "Client " << clientIP_ << " session started";

    return true;
}

string session::match(map<std::string, request_handler_factory*> routes, string& url) {
    
    std::string current_result = "/";

    for ( auto it = routes.begin(); it != routes.end(); ++it  )
    {
        std::string location = it->first;
        if (url.rfind(location, 0) == 0)
        {
            if (current_result.length() < location.length())
            {
                current_result = location;
            }
        }
    } 
	return current_result;
}

string session::handle_read(const boost::system::error_code& error,
		size_t bytes_transferred)
{
	BOOST_LOG_TRIVIAL(info) << "-- in handle_read";
	std::ostringstream ostring;
	ostring << request_;
	std::string request_string = ostring.str();
	BOOST_LOG_TRIVIAL(info) << "Request_string is " << request_string;
	BOOST_LOG_TRIVIAL(info) << "Request_string target is " << std::string(request_.target());
    string complete_request = "";

	if (!error)
	{
		string loc = std::string(request_.target());
		std::string location = match(routes_, loc);
		BOOST_LOG_TRIVIAL(info) << "TARGET Is ==== " << location;

		if (routes_.find(location) == routes_.end()) {
			BOOST_LOG_TRIVIAL(info) << "not found in routes_!";
			// 404 handler
		} else {
			BOOST_LOG_TRIVIAL(info) << "Exists here!!";

			request_handler_factory* factory = routes_[location];
			
			request_handler* handler = factory->create(location, loc);

			boost::beast::http::response <boost::beast::http::dynamic_body> response;

			handler->serve(request_, response);
			boost::beast::http::write(socket_, response);
		}
		complete_request = request_string;
		BOOST_LOG_TRIVIAL(info) << "parsed correctly\n";


		handle_write(error);
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "Client " << clientIP_ << " issues invalid request: " << complete_request;
		delete this;
	}
	return complete_request;
}


bool session::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
		boost::beast::http::async_read(socket_,
		buffer_,
		request_,
		boost::bind(&session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
		);
        BOOST_LOG_TRIVIAL(info) << "Client " << clientIP_ << " handle write complete";
        return true;
	}
	else
	{
        BOOST_LOG_TRIVIAL(info) << "Client " << clientIP_ << " handle write incomplete";
        return false;
	}
}
