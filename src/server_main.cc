//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <../src/server.cc>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <csignal>
#include <map>
#include "echo_handler_factory.h"
#include "config_parser.h"

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

void log_init()
{
	logging::add_console_log
	(
		std::cout,
		keywords::format = "[%TimeStamp%][%ThreadID%][%Severity%]: %Message%"
	);

	logging::add_file_log
	(
		keywords::file_name = "sample_%N.log",
		keywords::rotation_size = 10 * 1024 * 1024,
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::format = "[%TimeStamp%][%ThreadID%][%Severity%]: %Message%"
	);


	logging::core::get()->set_filter
	(
		logging::trivial::severity >= logging::trivial::info
	);
}

void signal_handler(int signal)
{
	BOOST_LOG_TRIVIAL(fatal) << "Received signal: " << signal << ", exiting\n";
	exit(signal);
}
 

int main(int argc, char* argv[])
{
	log_init();
	logging::add_common_attributes();
	std::signal(SIGINT, signal_handler);
	std::signal(SIGQUIT, signal_handler);
	
  	try
  	{
		if (argc != 2)
		{
			BOOST_LOG_TRIVIAL(error) << "Usage: async_tcp_echo_server <port>\n";
			return 1;
		}

		NginxConfigParser config_parser;
		NginxConfig config;
		if (!config_parser.Parse(argv[1], &config)) {
			BOOST_LOG_TRIVIAL(error) << "Invalid config file\n";
			return -1;
		}

		BOOST_LOG_TRIVIAL(info) << "ConfigParser: Parsing config file: " << argv[1] << "\n";

		int port = config.get_port_from_config(&config);
		BOOST_LOG_TRIVIAL(info) << "ConfigParser: Parsing complete\n";
		if (port == -1) {
			BOOST_LOG_TRIVIAL(error) << "ConfigParser: Invalid port number in config file\n";
			return -1;
		}

		boost::asio::io_service io_service;
		BOOST_LOG_TRIVIAL(info) << "Starting server on port: " << port << "\n";
		std::vector<path> paths = config.get_path_from_config();

		std::map<std::string, std::string> handler_names = config.get_handler_types();

		server s(io_service, port, paths, config, handler_names);
		BOOST_LOG_TRIVIAL(info) << "Accepting connections\n";

		io_service.run();
	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << "Exception: " << e.what() << "\n";
	}

	return 0;
}
