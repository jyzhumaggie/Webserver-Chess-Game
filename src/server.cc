#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "crud_handler_factory.h"
#include "session.h"
#include "server.h"
#include <boost/log/trivial.hpp>
#include "persistent_filesystem.h"

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service, short port, std::vector<path> paths, NginxConfig& config, std::map<std::string, std::string> handler_names, file_system* fs)
	: io_service_(io_service),
	acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
{  
	paths_ = paths;
	handler_names_ = handler_names;
	config_ = config;
	echo_handler_factory_ = nullptr;
	static_handler_factory_ = nullptr;	
	filesystem_ = fs;
	// build crud_endpoints_
	for(auto itr : paths){
		if(handler_names[itr.endpoint] == "crud_handler"){
			std::map<std::string, std::set<int>>* ptr = new std::map<std::string, std::set<int>>;
			filesystem_->list_files(itr.root, *ptr);
			crud_endpoints_[itr.endpoint] = ptr;
		}
	}
	start_accept();
}

server::~server(){
	for(auto itr : crud_endpoints_){
		delete itr.second;
	}
}

void server::create_handler_factory(const std::string& name, NginxConfig& config, const std::string& endpoint) {
	if (name == "echo_handler") {
		routes_[endpoint] = new echo_handler_factory(endpoint, config);
	} else if (name == "static_handler") {
		routes_[endpoint] = new static_handler_factory(endpoint, config);
	} else if (name == "crud_handler") {
		routes_[endpoint] = new crud_handler_factory(endpoint, config, crud_endpoints_[endpoint], filesystem_);
	} else if (name == "sleep_handler"){
		routes_[endpoint] = new sleep_handler_factory(endpoint, config);
	} else if (name == "health_handler"){
		routes_[endpoint] = new health_handler_factory(endpoint, config);
	} else {
		return ;
	}
}


void server::start_accept()
{
	session* new_session = new session(io_service_, config_, routes_);
	

	std::map<std::string, std::string>::iterator it = handler_names_.begin();
	BOOST_LOG_TRIVIAL(info) << "Start creating handler factories:\n";
	while (it != handler_names_.end())
	{
		std::string end = it->first;
		// handles optional trailing slash for endpoint
		if (end.back() != '/') {
			end += "/";
		}
		create_handler_factory(it->second, config_, end);
		++it;
	}
	new_session->set_routes(routes_);
	acceptor_.async_accept(new_session->socket(),
		boost::bind(&server::handle_accept, this, new_session,
		boost::asio::placeholders::error));
}

void server::generate_threads() {

	BOOST_LOG_TRIVIAL(info) << "Generating " << NUM_THREADS << " threads\n";
	// Launch the pool with four threads.
	boost::asio::thread_pool t_pool(NUM_THREADS);

	for (int i = 0; i < NUM_THREADS; i++) {
		// Submit io_service.run function to the pool.
		boost::asio::post(t_pool, boost::bind(&boost::asio::io_service::run, &io_service_));
	}
	t_pool.join();
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
