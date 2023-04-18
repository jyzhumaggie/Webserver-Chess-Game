#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"
#include <boost/regex.hpp>
#include "reply.h"

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

reply session::create_reply(string complete_request, int bytes_transferred)
{
    reply reply_;
    reply_.status = reply::ok; //http 200 response code
    reply_.content = complete_request;
    reply_.headers.resize(2);
    reply_.headers[0].name = "Content-Length";
    reply_.headers[0].value = std::to_string(bytes_transferred);
    reply_.headers[1].name = "Content-Type";
    reply_.headers[1].value = "text/plain"; //content type is text/plain
    return reply_;
}

void session::start()
{
  	boost::regex e("\r\n\r\n|\n\n");
	boost::asio::async_read_until(socket_, 
		request_, 
		e,
		boost::bind(&session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}


void session::handle_read(const boost::system::error_code& error,
		size_t bytes_transferred)
{
	if (!error)
	{
		istream is(&request_);
		string complete_request;
		while (complete_request.find("\r\n\r\n") == -1 && complete_request.find("\n\n") == -1 ) {
			char new_char;
			is.get(new_char);
			complete_request += new_char;
		}

		reply r = create_reply(complete_request, bytes_transferred);
		boost::asio::async_write(socket_,
				r.to_buffers(),
				boost::bind(&session::handle_write, this,
					boost::asio::placeholders::error));
	}
	else
	{
		delete this;
	}
}

void session::handle_write(const boost::system::error_code& error)
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
	}
	else
	{
		delete this;
	}
}
