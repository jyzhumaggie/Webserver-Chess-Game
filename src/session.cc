#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"
#include <boost/regex.hpp>
#include <boost/log/trivial.hpp>
#include <sstream>
#include "reply.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "not_found_handler.h"
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
    return true;
}

bool session::set_handler_names(std::map<std::string, std::string> handler_names)
{
    handler_names_ = handler_names;
    return true;
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

void session::handle_read(const boost::system::error_code& error,
		size_t bytes_transferred)
{
    string loc = std::string(request_.target());
	std::string location = match(routes_, loc);

	if (!error)
	{
		boost::beast::http::response <boost::beast::http::dynamic_body> response;
		if (routes_.find(location) == routes_.end()) {
			// 404 handler
			request_handler* handler = new not_found_handler(location, loc);
			handler->serve(request_, response);

            // machine-parsable log by regex statement: (\[ResponseMetrics\]: (\d+),([^,]+),([^,]+),([^,]+))
            // group 1: response code, group 2: request path, group 3: request IP, group 4: matched request handler name
            BOOST_LOG_TRIVIAL(info) << "[ResponseMetrics]: "<<std::to_string(response.result_int())<<","<<request_.target()<<","<<clientIP_<<","<<"not_found_handler";
		} else {
			request_handler_factory* factory = routes_[location];
			request_handler* handler = factory->create(location, loc);
			handler->serve(request_, response);
        
            // machine-parsable log by regex statement: (\[ResponseMetrics\]: (\d+),([^,]+),([^,]+),([^,]+))
            // group 1: response code, group 2: request path, group 3: request IP, group 4: matched request handler name
            BOOST_LOG_TRIVIAL(info) << "[ResponseMetrics]: "<<std::to_string(response.result_int())<<","<<request_.target()<<","<<clientIP_<<","<<handler_names_[location];	
		}

        
        boost::beast::http::write(socket_, response);

        delete this;
	}
	else
	{   
        boost::beast::http::response <boost::beast::http::dynamic_body> response;
        request_handler* handler = new error_handler(location, loc);
		handler->serve(request_, response);

        // machine-parsable log by regex statement: (\[ResponseMetrics\]: (\d+),([^,]+),([^,]+),([^,]+))
        // group 1: response code, group 2: request path, group 3: request IP, group 4: matched request handler name
        BOOST_LOG_TRIVIAL(info) << "[ResponseMetrics]: "<<std::to_string(response.result_int())<<","<<request_.target()<<","<<clientIP_<<","<<"error_handler";

        boost::beast::http::write(socket_,response);
        delete this;
	}
}
