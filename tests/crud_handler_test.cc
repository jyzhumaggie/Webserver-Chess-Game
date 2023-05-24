#include "crud_handler.h"
#include "mock_filesystem.h"
#include "gtest/gtest.h"
#include "config_parser.h"
#include <boost/beast/http.hpp>

class CrudHandlerTestFixture : public ::testing::Test {
protected:
	mock_filesystem mfs;
	NginxConfig config;
	NginxConfigParser parser;
	std::string location = "/resource1/";
	std::string request_url = "/resource1/Shoes";
	std::map<std::string, std::set<int>> entities;
	boost::beast::http::request<boost::beast::http::dynamic_body> req;
	boost::beast::http::response<boost::beast::http::dynamic_body> res;
};

TEST_F(CrudHandlerTestFixture, GetInvalidEntity) {
	std::string location;
	std::string request_url;
	crud_handler c(location, request_url, config, &entities, &mfs);
	req.method(boost::beast::http::verb::get);
	req.target("/resource1/Shoes/1");
  	c.serve(req, res);
  	EXPECT_EQ(boost::beast::buffers_to_string(res.body().data()), "Invalid entity");
}

TEST_F(CrudHandlerTestFixture, GetValidEntity) {
	entities["Shoes"] = {0,1,2,3,4};

	parser.Parse("../example_config", &config);
	crud_handler c(location, request_url, config, &entities, &mfs);

	req.method(boost::beast::http::verb::get);
	req.target("/resource1/Shoes");
  	c.serve(req, res);

  	EXPECT_EQ(res.result(), boost::beast::http::status::ok);
  	EXPECT_EQ(boost::beast::buffers_to_string(res.body().data()), "[0, 1, 2, 3, 4]");
}

TEST_F(CrudHandlerTestFixture, PutInvalidEntity) {
	parser.Parse("../example_config", &config);
	crud_handler c(location, request_url, config, &entities, &mfs);

	beast::ostream(req.body()) << "{\"id\":456}";
	req.method(boost::beast::http::verb::put);
	req.target("/resource1/Shoes");

  	c.serve(req, res);
  	EXPECT_EQ(res.result(), boost::beast::http::status::bad_request);
  	EXPECT_EQ(boost::beast::buffers_to_string(res.body().data()),  "No id specified in CRUD update");
}

TEST_F(CrudHandlerTestFixture, PutExistingEntity) {
	std::map<std::string, nlohmann::json> files_;
	nlohmann::json init_data;
	init_data["id"] = "123";
	files_["/resource1/Shoes/1"] = init_data;
	mock_filesystem mfs(files_);

	parser.Parse("../example_config", &config);

	request_url += "/1";
	entities["Shoes"].insert(1);
	crud_handler c(location, request_url, config, &entities, &mfs);

	beast::ostream(req.body()) << "{\"id\":456}";
	req.method(boost::beast::http::verb::put);
	req.target("/resource1/Shoes/1");

  	c.serve(req, res);
  	EXPECT_EQ(res.result(), boost::beast::http::status::ok);
  	EXPECT_EQ(boost::beast::buffers_to_string(res.body().data()), "{\"id\": 1}");
}

TEST_F(CrudHandlerTestFixture, PutNewEntity) {
	parser.Parse("../example_config", &config);

	request_url += "/1";
	entities["Shoes"].insert(0);
	crud_handler c(location, request_url, config, &entities, &mfs);

	beast::ostream(req.body()) << "{\"id\":456}";
	req.method(boost::beast::http::verb::put);
	req.target("/resource1/Shoes/1");

  	c.serve(req, res);
  	EXPECT_EQ(res.result(), boost::beast::http::status::created);
  	EXPECT_EQ(boost::beast::buffers_to_string(res.body().data()), "{\"id\": 1}");
}

TEST_F(CrudHandlerTestFixture, DeleteEntity) {
	std::map<std::string, nlohmann::json> files_;
	nlohmann::json data;
	data["id"] = "123";
	files_["/resource1/Shoes/0"] = data;
	mock_filesystem mfs(files_);
	
	parser.Parse("../example_config", &config);

	entities["Shoes"].insert(0);
	crud_handler c(location, request_url, config, &entities, &mfs);

	beast::ostream(req.body()) << "{\"id\":123}";
	req.method(boost::beast::http::verb::delete_);
	req.target("/resource1/Shoes");

  	c.serve(req, res);
  	EXPECT_EQ(res.result(), boost::beast::http::status::ok);
  	EXPECT_EQ(boost::beast::buffers_to_string(res.body().data()), "Deleted 0");
}

TEST_F(CrudHandlerTestFixture, DeleteInvalidEntity) {
	std::map<std::string, nlohmann::json> files_;
	nlohmann::json data;
	data["id"] = "123";
	files_["/resource1/Shoes/0"] = data;
	mock_filesystem mfs(files_);
	
	parser.Parse("../example_config", &config);

	entities["Shoes"].insert(1);
	crud_handler c(location, request_url, config, &entities, &mfs);

	beast::ostream(req.body()) << "{\"id\":123}";
	req.method(boost::beast::http::verb::delete_);
	req.target("/resource1/Shoes");

  	c.serve(req, res);
  	EXPECT_EQ(res.result(), boost::beast::http::status::not_found);
  	EXPECT_EQ(boost::beast::buffers_to_string(res.body().data()), "Invalid id specified in CRUD delete");
}


TEST_F(CrudHandlerTestFixture, PostEntity) {
	std::map<std::string, nlohmann::json> files_;
	parser.Parse("../example_config", &config);
	crud_handler c(location, request_url, config, &entities, &mfs);

	beast::ostream(req.body()) << "{\"id\":456}";
	req.method(boost::beast::http::verb::post);
	req.target("/resource1/Shoes");

  	c.serve(req, res);
  	EXPECT_EQ(res.result(), boost::beast::http::status::created);
  	EXPECT_EQ(boost::beast::buffers_to_string(res.body().data()), "{\"id\": 1}");
}

