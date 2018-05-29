#include "web_server.h"
#include "catch.hpp"
using namespace std;

bool operator== (const webserver::http_header &header1, const webserver::http_header &header2) {
    return (header1.type == header2.type && header1.value == header2.value);
}

TEST_CASE("ParseHeaders_RawRequest_ParsedHeaders", "Parser") {
    vector<string> raw_request;

    raw_request.emplace_back("GET /tutorials/other/top-20-mysql-best-practices/ HTTP/1.1");
    raw_request.emplace_back("Host: net.tutsplus.com");
    raw_request.emplace_back("User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)");
    raw_request.emplace_back("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    raw_request.emplace_back("\r\n");

    webserver::http_request proper_request;

    webserver::http_header header;

    header = {"Host", "net.tutsplus.com"};
    proper_request.add_http_request_header(header);
    header = {"User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)"};
    proper_request.add_http_request_header(header);
    header = {"Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"};
    proper_request.add_http_request_header(header);

    webserver::http_request_parser parser;
    webserver::http_request received_request = parser.parse_request(raw_request);

    const vector<webserver::http_header>& proper_request_headers = proper_request.get_headers();
    const vector<webserver::http_header>& received_request_headers = received_request.get_headers();

    for (unsigned long current_header_number = 0; current_header_number < proper_request_headers.size(); current_header_number++) {
//        REQUIRE(proper_request_headers[current_header_number] == received_request_headers[current_header_number]);
        REQUIRE(received_request_headers[current_header_number].type == proper_request_headers[current_header_number].type);
        REQUIRE(received_request_headers[current_header_number].value == proper_request_headers[current_header_number].value);
    }
}

TEST_CASE("ParseRequestBody_RawRequest_ParsedRequestBody", "Parser") {
    vector<string> raw_request;

    string request_body = "Hello, World!";
    unsigned long request_body_size = request_body.size();
    raw_request.emplace_back("GET /tutorials/other/top-20-mysql-best-practices/ HTTP/1.1");
    raw_request.emplace_back("Host: net.tutsplus.com");
    raw_request.emplace_back("Content-Length: " + to_string(request_body_size));
    raw_request.emplace_back("Content-Type: text/html; charset=utf-8");
    raw_request.emplace_back("\r\n");
    raw_request.emplace_back(request_body);

    webserver::http_request proper_request;

    proper_request.set_http_request_method("GET");
    proper_request.set_http_request_url("net.tutsplus.com/tutorials/other/top-20-mysql-best-practices/");
    webserver::http_header header;
    header.type = "Host";
    header.value = "net.tutsplus.com";
    proper_request.add_http_request_header(header);
    header.type = "Content-type";
    header.value = "utf-8";
    proper_request.add_http_request_header(header);
    proper_request.set_http_request_body("Hello, World!");

    webserver::http_request received_request;

    webserver::http_request_parser parser;
    received_request = parser.parse_request(raw_request);

    REQUIRE(proper_request.get_request_body() == received_request.get_request_body());
}