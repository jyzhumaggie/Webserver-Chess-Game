#include "gtest/gtest.h"
#include "session.h"
#include "gmock/gmock.h"

class SessionFixture : public ::testing::Test {
 protected:
    boost::asio::io_service io_service;
    NginxConfig config_;
    std::map<std::string, request_handler_factory*> routes_;
};

//Tests session starts successfully
TEST_F(SessionFixture, SessionStart){
    session s(io_service, config_, routes_);
    std::vector<path> paths;
    EXPECT_TRUE(s.start(paths));
}

//Tests match function
TEST_F(SessionFixture, Match){
    session s(io_service, config_, routes_);
    map<std::string, request_handler_factory*> routes;
    routes["/static1/"] = nullptr;
    std::string url = "/static1/";
    EXPECT_EQ(s.match(routes,url),"/static1/");
}

//Tests set routes function
TEST_F(SessionFixture, SetRoutes){
    session s(io_service, config_, routes_);
    std::map<std::string, request_handler_factory*> routes;
    EXPECT_TRUE(s.set_routes(routes));
}

//Set Handler Names
TEST_F(SessionFixture, SetHandlerNames){
    session s(io_service, config_, routes_);
    std::map<std::string, std::string> handler_names;
    EXPECT_TRUE(s.set_handler_names(handler_names));
}

//Tests socket function
TEST_F(SessionFixture, Socket){
    session s(io_service, config_, routes_);
    tcp::socket& socket = s.socket();
    EXPECT_FALSE(socket.is_open());
}
