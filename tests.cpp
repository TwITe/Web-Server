#include "web_server.h"
#include "catch.hpp"
#define CATCH_CONFIG_MAIN
using namespace std;

TEST_CASE("Parser request line function works well", "[Parser]") {
    vector <string> request_line;
    request_line.push_back("GET /tutorials/other/top-20-mysql-best-practices/ HTTP/1.1");
    webserver::http_request request;
    request.set_http_request_method("GET");
    request.set_http_request_url("/tutorials/other/top-20-mysql-best-practices/");
    webserver::http_request received_request;
    webserver::http_request_parser parser(request_line);
    received_request = parser.parse();

    REQUIRE();
}