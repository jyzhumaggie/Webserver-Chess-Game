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
#include <boost/log/trivial.hpp>
#include <string>

using boost::asio::ip::tcp;
using namespace std;

session::session(boost::asio::io_service& io_service)
		: socket_(io_service)
{
}

tcp::socket& session::socket()
{
	return socket_;
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

  	boost::regex e("\r\n\r\n|\n\n");
	boost::asio::async_read_until(socket_, 
		request_, 
		e,
		boost::bind(&session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));


    BOOST_LOG_TRIVIAL(info) << "Client " << clientIP_ << " session started";

    return true;
}


string session::handle_read(const boost::system::error_code& error,
		size_t bytes_transferred)
{
    string complete_request = "";
	if (!error)
	{
		istream is(&request_);
		while (complete_request.find("\r\n\r\n") == -1 && complete_request.find("\n\n") == -1 ) {
			char new_char;
			is.get(new_char);
			complete_request += new_char;
		}

		file_handler handler("./",paths_);
		reply new_reply;
		if (handler.parse(complete_request)) {
			//serve file
			handler.handle_request(socket_);
			BOOST_LOG_TRIVIAL(info)<< "Client " << clientIP_ << " issues valid request: " << complete_request;
		}
		else {
			// echo_handler echoer("",paths_);
			echo_handler echoer("");
			echoer.parse(complete_request);
			echoer.handle_request(socket_);
			BOOST_LOG_TRIVIAL(info) << "Client " << clientIP_ << " issues invalid request: " << complete_request;
		}
		handle_write(error);
	}
	else
	{
        BOOST_LOG_TRIVIAL(info)<<"Client "<<clientIP_<<" handle read incomplete";
	}
	return complete_request;
}

bool session::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
		boost::regex e("\r\n\r\n|\n\n");

		boost::asio::async_read_until(socket_,
		request_, 
		e,
		boost::bind(&session::handle_read, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
        
        BOOST_LOG_TRIVIAL(info)<<"Client "<<clientIP_<<" handle write complete";
        return true;
	}
	else
	{
        BOOST_LOG_TRIVIAL(info)<<"Client "<<clientIP_<<" handle write incomplete";
        return false;
	}
}
