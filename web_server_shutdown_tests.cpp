#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>
#include "catch.hpp"
#include "web_server.h"
#include <cpr/cpr.h>
using namespace std;

const unsigned short int PORT = 8080;

_Pragma("GCC diagnostic push")
_Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
function<webserver::http_response(webserver::http_request)> health_check = [&](webserver::http_request request) {
    webserver::http_response response;

    string response_body = "Hello, World!";

    response.set_response_body(response_body);
    response.set_response_http_code(200);
    response.set_response_length(response_body.length());

    return response;
};
_Pragma("GCC diagnostic pop")

webserver::web_handler health_check_handler("/is_server_up", "GET", health_check);

vector<webserver::web_handler> handlers{health_check_handler};

TEST_CASE("Connect single client", "[Shutdown Tests]") {
    webserver::web_server server(PORT, handlers);
    thread server_run = thread([&]{server.start();});
    server_run.detach();

    auto r = cpr::Get(cpr::Url("http://localhost:8080/is_server_up"));

    server.stop();

    r = cpr::Get(cpr::Url("http://localhost:8080/is_server_up"));

    REQUIRE(r.error.code == cpr::ErrorCode::CONNECTION_FAILURE);
}