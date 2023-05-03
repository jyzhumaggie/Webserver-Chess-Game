#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"
#include "server.h"

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service, short port,std::vector<path> paths)
	: io_service_(io_service),
	acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
{  
        paths_ = paths;
		start_accept();
}

void server::start_accept()
{
	session* new_session = new session(io_service_);
	acceptor_.async_accept(new_session->socket(),
		boost::bind(&server::handle_accept, this, new_session,
		boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session, const boost::system::error_code& error)
{
	if (!error)
	{
		new_session->start(paths_);
	}
	else
	{
		delete new_session;
	}

	start_accept();
}
