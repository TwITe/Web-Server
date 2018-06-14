#include "web_server.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("ParseHeaders_RawRequest_ParsedHeaders", "Parser") {
    vector<string> raw_request;

    raw_request.emplace_back("GET /tutorials/other/top-20-mysql-best-practices/ HTTP/1.1\r\n");
    raw_request.emplace_back("Host: net.tutsplus.com\r\n");
    raw_request.emplace_back("User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)\r\n");
    raw_request.emplace_back("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n");
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
        REQUIRE(received_request_headers[current_header_number].type == proper_request_headers[current_header_number].type);
        REQUIRE(received_request_headers[current_header_number].value == proper_request_headers[current_header_number].value);
    }
}

TEST_CASE("ParseRequestLine_RawRequest_ParsedRequestLine") {
    vector<string> raw_request;

    raw_request.emplace_back("GET /feed HTTP/1.1\r\n");
    raw_request.emplace_back("Host: vk.com\r\n");
    raw_request.emplace_back("\r\n");

    webserver::http_request proper_request;
    proper_request.set_http_request_url("vk.com/feed");
    proper_request.set_http_request_method("GET");
    proper_request.set_http_version("HTTP/1.1");

    webserver::http_request_parser parser;
    webserver::http_request received_request = parser.parse_request(raw_request);

    REQUIRE(proper_request.get_request_method() == received_request.get_request_method());
    REQUIRE(proper_request.get_request_url() == received_request.get_request_url());
    REQUIRE(proper_request.get_request_http_version() == received_request.get_request_http_version());
}

TEST_CASE("ParseUrlencodedRequestBody_RawUrlencodedBody_ParsedUrlencodedBody", "Parser") {
    vector<string> raw_request;

    string request_body = "say=Hi!";
    raw_request.emplace_back("POST / HTTP/1.1\r\n");
    raw_request.emplace_back("Host: foo.com\r\n");
    raw_request.emplace_back("Content-Type: application/x-www-form-urlencoded; charset=\"utf-8\"\r\n");
    raw_request.emplace_back("Content-Length: 13\r\n");
    raw_request.emplace_back("\r\n");
    raw_request.emplace_back(request_body);

    webserver::http_request proper_request;

    proper_request.add_request_body_field("say", "Hi!");

    webserver::http_request_parser parser;
    webserver::http_request received_request = parser.parse_request(raw_request);

    REQUIRE(proper_request.get_request_body() == received_request.get_request_body());
}

TEST_CASE("ParseUrlencodedRequestBodyW/MultipleParameters_RawUrlencodedBody_ParsedUrlencodedBody", "Parser") {
    vector<string> raw_request;

    string request_body = "say=Hi&to=Mom&from=Son";
    raw_request.emplace_back("POST / HTTP/1.1\r\n");
    raw_request.emplace_back("Host: foo.com\r\n");
    raw_request.emplace_back("Content-Type: application/x-www-form-urlencoded; charset=\"utf-8\"\r\n");
    raw_request.emplace_back("Content-Length: 13\r\n");
    raw_request.emplace_back("\r\n");
    raw_request.emplace_back(request_body);

    webserver::http_request proper_request;

    proper_request.add_request_body_field("say", "Hi");
    proper_request.add_request_body_field("to", "Mom");
    proper_request.add_request_body_field("from", "Son");

    webserver::http_request_parser parser;
    webserver::http_request received_request = parser.parse_request(raw_request);

    REQUIRE(proper_request.get_request_body() == received_request.get_request_body());
}

TEST_CASE("ParseFormDataRequestBody_RawFormDataBodyw/EndBoundary_ParsedFormDataBody", "Parser") {
    vector<string> raw_request;

    string request_body = "--boundary\r\n"
                          "Content-Disposition: form-data; name=\"field1\"\r\n"
                          "\r\n"
                          "value1\r\n"
                          "--boundary\r\n"
                          "Content-Disposition: form-data; name=\"field2\"; filename=\"example.txt\"\r\n"
                          "\r\n"
                          "value2\r\n"
                          "--boundary--\r\n"
                          "Content-Disposition: form-data; name=\"field3\"\r\n"
                          "\r\n"
                          "value3";

    raw_request.emplace_back("POST /test.html HTTP/1.1\r\n");
    raw_request.emplace_back("Host: example.org\r\n");
    raw_request.emplace_back("Content-Type: multipart/form-data;boundary=\"boundary\"\r\n");
    raw_request.emplace_back("\r\n");
    raw_request.emplace_back(request_body);

    webserver::http_request proper_request;

    proper_request.add_request_body_field("field1", "value1");
    proper_request.add_request_body_field("field2", "value2");

    webserver::http_request_parser parser;
    webserver::http_request received_request = parser.parse_request(raw_request);

    REQUIRE(proper_request.get_request_body() == received_request.get_request_body());
}

TEST_CASE("ParseFormDataRequestBody_RawFormDataBodyw/oEndBoundary_ParsedFormDataBody", "Parser") {
    vector<string> raw_request;

    string request_body = "--boundary\r\n"
                          "Content-Disposition: form-data; name=\"field1\"\r\n"
                          "\r\n"
                          "value1\r\n"
                          "--boundary\r\n"
                          "Content-Disposition: form-data; name=\"field2\"; filename=\"example.txt\"\r\n"
                          "\r\n"
                          "value2";

    raw_request.emplace_back("POST /test.html HTTP/1.1\r\n");
    raw_request.emplace_back("Host: example.org\r\n");
    raw_request.emplace_back("Content-Type: multipart/form-data;boundary=\"boundary\"\r\n");
    raw_request.emplace_back("\r\n");
    raw_request.emplace_back(request_body);

    webserver::http_request proper_request;

    proper_request.add_request_body_field("field1", "value1");
    proper_request.add_request_body_field("field2", "value2");

    webserver::http_request_parser parser;
    webserver::http_request received_request = parser.parse_request(raw_request);

    REQUIRE(proper_request.get_request_body() == received_request.get_request_body());
}

TEST_CASE("ParseRequestParameters_RawRequest_ParsedUrlParameters", "Parser") {
    vector<string> raw_request;

    raw_request.emplace_back("GET /?clid=2063711&clid=23141 HTTP/1.1");
    raw_request.emplace_back("Host: yandex.ru");
    raw_request.emplace_back("\r\n");

    webserver::http_request proper_request;

    webserver::request_param request_parameter;
    request_parameter = {"clid", "2063711"};
    proper_request.add_http_request_param(request_parameter);
    request_parameter = {"clid", "23141"};
    proper_request.add_http_request_param(request_parameter);

    webserver::http_request_parser parser;
    webserver::http_request received_request = parser.parse_request(raw_request);

    const vector<webserver::request_param>& received_request_parameters = received_request.get_request_params();
    const vector<webserver::request_param>& proper_request_parameters = proper_request.get_request_params();

    for (unsigned long current_request_parameter_number = 0; current_request_parameter_number < proper_request_parameters.size(); current_request_parameter_number++) {
        REQUIRE(received_request_parameters[current_request_parameter_number].name == proper_request_parameters[current_request_parameter_number].name);
        REQUIRE(received_request_parameters[current_request_parameter_number].value == proper_request_parameters[current_request_parameter_number].value);
    }
}

TEST_CASE("ExtendRequestUrlByHost_AbsolutePath_ConvertToAbsoluteUrl", "Parser") {
    vector<string> raw_request;

    raw_request.emplace_back("GET /feed HTTP/1.1\r\n");
    raw_request.emplace_back("Host: vk.com\r\n");
    raw_request.emplace_back("\r\n");

    webserver::http_request proper_request;
    proper_request.set_http_request_url("vk.com/feed");

    webserver::http_request_parser parser;
    webserver::http_request received_request = parser.parse_request(raw_request);

    const string& proper_request_url = proper_request.get_request_url();
    const string& received_request_url = received_request.get_request_url();

    REQUIRE(proper_request_url == received_request_url);
}