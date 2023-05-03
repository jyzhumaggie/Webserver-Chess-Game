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

bool session::start()
{
  	boost::regex e("\r\n\r\n|\n\n");
	boost::asio::async_read_until(socket_, 
		request_, 
		e,
		boost::bind(&session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

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

		file_handler handler("../files/static/");
		reply new_reply;
		if (handler.parse(complete_request)) {
			//serve file
			handler.handle_request(socket_);
		}
		else {
			echo_handler echoer("");
			echoer.parse(complete_request);
			echoer.handle_request(socket_);
		}
		handle_write(error);
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
        return true;
	}
	else
	{
        return false;
	}
}
