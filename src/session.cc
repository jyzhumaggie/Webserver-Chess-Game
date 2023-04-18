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

void session::start()
{
	// socket_.async_read_some(boost::asio::buffer(data_, max_length),
	// 		boost::bind(&session::handle_read, this,
	// 			boost::asio::placeholders::error,
	// 			boost::asio::placeholders::bytes_transferred));
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

		// cout << "data is : " << bytes_transferred << endl;
		// reply r = create_reply(complete_request, bytes_transferred);
		// boost::asio::async_write(socket_,
		// 		r.to_buffer(),
		// 		boost::bind(&session::handle_write, this,
		// 			boost::asio::placeholders::error));
		boost::asio::async_write(socket_,
				boost::asio::buffer(complete_request, bytes_transferred),
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
		// socket_.async_read_some(boost::asio::buffer(data_, max_length),
		// 		boost::bind(&session::handle_read, this,
		// 			boost::asio::placeholders::error,
		// 			boost::asio::placeholders::bytes_transferred));
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
