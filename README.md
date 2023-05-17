# Repository Structure
- chickens-and-more: top-level root of repository
    - config: stores testing and deployment configuration files
    - docker: stores docker files for generating images
    - files: stores the files used for testing the static handler functionality
    - include: contains all header files, documentation detailing the purpose of each header file with respect to the overall structure of codebase is in the comment of the respective header files 
    - src: contains all source code files
    - tests: contains all testing files
        - ConfigParserTestFiles: contains config files to test config_parser module
        - integration_test.sh: contains integration test script. The expected outputs for the integration tests are stored in integration_testN_result. Newer versions of the test results keep on being added as we make changes to the codebase, so refer to largest integer number N for latest expected out of bash testing script.
    - example_config: example configuration file for our testing purposes.


# Building the Code
1. At the top level chickens-and-more directory, create a build directory by using command `mkdir build`.
2. Go into the build directory by using command `cd build` then use command `cmake ..` to configure and create the relevant build files.
3. Use command `make` to build and link the program.


# Testing the Code
1. Navigate to the build directory.
2. Use the command `ctest` or `make test` to run the unit and integration tests. 
3. To see a more detailed version of testing outputs, run the command `ctest -V`.


# Generating Code Coverage Report
1. If you wish to view code coverage report, navigate to the build folder in the repo.
2. Run `cmake -DCMAKE_BUILD_TYPE=Coverage ..` followed by `make coverage` to build and link the program.


# Running the Code
1. Navigate to the build directory.
2. Use the command `./bin/webserver <path_to_config>` to launch the web server (the config path should usually be `../config/<your_config>` assuming that your configuration file is placed there).


# Building and Running the Docker Image
1. Navigate to the `chickens-and-more` directory.
2. Use `docker build -f docker/base.Dockerfile -t base .` to build the base image.
3. Use `docker build -f docker/Dockerfile -t server .` to build the web server image.
4. Use `docker build -f docker/coverage.Dockerfile -t coverage .` to build the coverage image.
5. Use `docker run --rm -p <test_port>:80 --name server_run server:latest` to run the docker image locally.

# Important Files
**chickens-and-more**
- CMakeLists.txt: New handlers/factories should be referenced here in order to build and link the webserver binary correctly.

**config**
- example.conf: Configuration file for local testing.
- integration_test.conf: Configuration for integration test.

**src**
- config_parser.cc: New endpoint types should be added in the configuration parser.
- server.cc: New factories should be added to the create_handler_factory function.

More details on adding request handlers and factories is at a later point in this README file.

# Config File Format
In the config folder there is an example.conf file that shows how to write the configuration file for our webserver. It starts by declaring the port that the server will listen to with the keyword `port`. For the example.conf it listens on port 80 and for integration test it listens to the reserved port 800. Port 8080 can also be used for the test config.

The rest of the statements for both are “location” statements conform to the “location-major typed” format. In these statements, the location keyword is followed by the serving path, followed by the name of the handler, and possible arguments inside `{...}`. 
Right now, the static handler statements have a root argument that declares the directory where the handler will serve files from, and the echo handler statement does not have any arguments because there is not any need at the moment. When adding another handler, make sure to follow the format explained above and used the example.conf file.


# RequestHandler Interface
NEEDS TO BE COMPLETED

## Example of a RequestHandler Child Class
**echo_handler.h**
```
#ifndef ECHO_HANDLER_H
#define ECHO_HANDLER_H

#include <string>
#include <vector>
#include "request_handler.h"
#include "header.h"
#include "reply.h"

#include <boost/asio.hpp>
#include <iostream>

using namespace http::server;
using boost::asio::ip::tcp;

/**
 * echo_handler class
 *
 * Inherits from request_handler class
 *
 * The echo handler constructs proper echo response based on root directory 
 * and vector of paths that is provided to it
 *
 * The function handle_request constructs the echo response
 * with the proper status codes, content, and headers.
 */

class echo_handler : public request_handler {
    public:
        echo_handler(std::string base_dir);
        void handle_request(tcp::socket& socket);
};

#endif
```

**echo_handler.cc**
```
#include "echo_handler.h"
#include "request_handler.h"

echo_handler::echo_handler(std::string base_dir) : request_handler(base_dir,paths_) {
}

void echo_handler::handle_request(tcp::socket& socket) {
    reply reply_;
    reply_.status = reply::ok;
    reply_.content = complete_request;
    reply_.headers.resize(2);
    reply_.headers[0].name = "Content-Length";
    reply_.headers[0].value = std::to_string(complete_request.size());
    reply_.headers[1].name = "Content-Type";
    reply_.headers[1].value = "text/html";
    boost::asio::write(socket, reply_.to_buffers());
}
```
The different handler constructors will take in different values depending on what is needed. For example, the echo location takes in 1 string parameter: the location of the files to be served.

## Adding Another RequestHandler
To add another request handler, create a corresponding header and source file in the request_handler folders. In the header file, include the parent class `request_handler.h`. Next, make sure to make request_handler public when declaring the class. Additionally, make the constructor and the inherited handle_request function public as well, and make any additional objects private.

In the source file, include the header file of this new request handler and any other useful header file or libraries. Then implement the constructor and the handle_request function.



# RequestHandlerFactory Interface
The request handler factory interface consists of a create function. The create function is a pure virtual function and it is inherited and overridden by other request handler factories in the program. The create function takes in a string with the location and an nginx config file and returns a request_handler pointer.

## Example of a RequestHandlerFactory Child Class
**echo_handler_factory.h**
```
#ifndef ECHO_HANDLER_FACTORY_H
#define ECHO_HANDLER_FACTORY_H

#include "request_handler_factory.h"
#include <string>
#include "config_parser.h"
#include "request_handler.h"
#include "echo_handler.h"

/**
 * echo_handler_factory class inherits from request_handler_factory class
 *
 * the echo handler factory class constructs an echo handler factory object by giving the 
 * config file and path
 *
 * the create function will return a request_handler point, which in this class points to an echo handler 
 */

class echo_handler_factory : public request_handler_factory {
    public:
    echo_handler_factory(const std::string& path, NginxConfig& config);
    request_handler* create(const std::string& location,NginxConfig& config);

};


#endif
```

**echo_handler_factory.cc**
```
#include "echo_handler_factory.h"


echo_handler_factory::echo_handler_factory(const std::string& path, NginxConfig& config) {
    
}

request_handler* echo_handler_factory::create(const std::string& location,NginxConfig& config)
{
    //TODO: will need to be later changed to new echo_handler(const std::string& location,NginxConfig& config)
    return new echo_handler("");
}

```

The constructor of the request_handler_factory takes a `string` object *location* and a `NginxConfig` object *config* to construct a request_handler_factory object. For example the echo_handler_factory constructor will construct a `echo_handler_factory` object by giving it an *location* and *config*.
The create function is inherited from the request_handler_factory interface and it returns a corresponding handler pointer. In `echo_handler_factory`, the create function takes in a string and an NginxConfig object *location* and *config* respectively and it returns an `echo_handler` pointer.



## Adding Another RequestHandlerFactory
NEEDS TO BE DONE
