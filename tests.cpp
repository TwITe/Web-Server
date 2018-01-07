#define CATCH_CONFIG_MAIN
#include "web_server.h"
#include "catch.hpp"
#include "http_request_parser.h"

using namespace std;

TEST_CASE("Parser works well (case 1)", "[Parser]") {
    vector<string> message_for_parse;

    message_for_parse.emplace_back("GET /tutorials/other/top-20-mysql-best-practices/ HTTP/1.1");
    message_for_parse.emplace_back("Host: net.tutsplus.com");
    message_for_parse.emplace_back("User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)");
    message_for_parse.emplace_back("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");

    webserver::http_request proper_request;

    proper_request.set_http_request_method("GET");
    proper_request.set_http_request_url("net.tutsplus.com/tutorials/other/top-20-mysql-best-practices/");
    webserver::http_header header;
    header.type = "Host";
    header.value = "net.tutsplus.com";
    proper_request.add_http_request_header(header);
    header.type = "User-Agent";
    header.value = "Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)";
    proper_request.add_http_request_header(header);
    header.type = "Accept";
    header.value = "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";
    proper_request.add_http_request_header(header);

    webserver::http_request received_request;

    webserver::http_request_parser parser;
    received_request = parser.parse(message_for_parse);

    vector<webserver::http_header> proper_request_headers = proper_request.get_headers();
    vector<webserver::http_header> received_request_headers = received_request.get_headers();

    for (int current_header_number = 0; current_header_number < proper_request_headers.size(); current_header_number++) {
        REQUIRE(proper_request_headers[current_header_number].type == received_request_headers[current_header_number].type);
        REQUIRE(proper_request_headers[current_header_number].value == received_request_headers[current_header_number].value);
    }

    REQUIRE(proper_request.get_request_method() == received_request.get_request_method());
    REQUIRE(proper_request.get_request_url() == received_request.get_request_url());
}