#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"
#include <boost/regex.hpp>
#include <sstream>
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

		reply r;
        reply new_reply = r.create_reply(complete_request, bytes_transferred);
		boost::asio::async_write(socket_,
				new_reply.to_buffers(),
				boost::bind(&session::handle_write, this,
					boost::asio::placeholders::error));
	}
	else
	{
        return complete_request;
	}
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
