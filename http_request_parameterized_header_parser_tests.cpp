#include "web_server.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("ParseContentTypeHeaderW/oParameters_RawContentTypeHeader_ParsedContentTypeHeader", "Parameterized_Headers_Parser") {
    const string content_type_header = "application/x-www-form-urlencoded";
    webserver::http_request_parameterized_header_parser header_parser;

    const pair<string, map<string, string>>& proper_parsed_header{"application/x-www-form-urlencoded", map<string, string>{}};

    const pair<string, map<string, string>>& received_parsed_header = header_parser.parse_parameterized_header(content_type_header);

    REQUIRE(proper_parsed_header.first == received_parsed_header.first);
}

TEST_CASE("ParseContentTypeHeaderW/ParametersW/oWhitespaces_RawContentTypeHeader_ParsedContentTypeHeader", "Parameterized_Headers_Parser") {
    const string content_type_header = "application/x-www-form-urlencoded;charset=utf8";
    webserver::http_request_parameterized_header_parser header_parser;

    const pair<string, map<string, string>>& proper_parsed_header{"application/x-www-form-urlencoded", {{"charset", "utf8"}}};

    const pair<string, map<string, string>>& received_parsed_header = header_parser.parse_parameterized_header(content_type_header);

    REQUIRE(proper_parsed_header.first == received_parsed_header.first);
    REQUIRE(proper_parsed_header.second == received_parsed_header.second);
}

TEST_CASE("ParseContentTypeHeaderW/ParametersW/Whitespaces_RawContentTypeHeader_ParsedContentTypeHeader", "Parameterized_Headers_Parser") {
    const string content_type_header = "application/x-www-form-urlencoded;   charset=utf8";
    webserver::http_request_parameterized_header_parser header_parser;

    const pair<string, map<string, string>>& proper_parsed_header{"application/x-www-form-urlencoded", {{"charset", "utf8"}}};

    const pair<string, map<string, string>>& received_parsed_header = header_parser.parse_parameterized_header(content_type_header);

    REQUIRE(proper_parsed_header.first == received_parsed_header.first);
    REQUIRE(proper_parsed_header.second == received_parsed_header.second);
}

TEST_CASE("ParseContentTypeHeaderW/ParametersW/WhitespacesW/Quotes_RawContentTypeHeader_ParsedContentTypeHeader", "Parameterized_Headers_Parser") {
    const string content_type_header = "application/x-www-form-urlencoded;   charset=\"utf8\"";
    webserver::http_request_parameterized_header_parser header_parser;

    const pair<string, map<string, string>>& proper_parsed_header{"application/x-www-form-urlencoded", {{"charset", "utf8"}}};

    const pair<string, map<string, string>>& received_parsed_header = header_parser.parse_parameterized_header(content_type_header);

    REQUIRE(proper_parsed_header.first == received_parsed_header.first);
    REQUIRE(proper_parsed_header.second == received_parsed_header.second);
}

TEST_CASE("ParseContentDispositionHeaderW/ParametersW/WhitespacesW/oQuotes_RawContentDispositionHeader_ParsedContentDispositionHeader", "Parameterized_Headers_Parser") {
    const string content_disposition_header = "form-data; name=say; filename=message.txt";
    webserver::http_request_parameterized_header_parser header_parser;

    const pair<string, map<string, string>>& proper_parsed_header{"form-data", {{"name", "say"}, {"filename", "message.txt"}}};

    const pair<string, map<string, string>>& received_parsed_header = header_parser.parse_parameterized_header(content_disposition_header);

    REQUIRE(proper_parsed_header.first == received_parsed_header.first);
    REQUIRE(proper_parsed_header.second == received_parsed_header.second);
}

TEST_CASE("ParseContentDispositionHeaderW/ParametersW/WhitespacesW/Quotes_RawContentDispositionHeader_ParsedContentDispositionHeader", "Parameterized_Headers_Parser") {
    const string content_disposition_header = "form-data; name=\"say\"; filename=\"message.txt\"";
    webserver::http_request_parameterized_header_parser header_parser;

    const pair<string, map<string, string>>& proper_parsed_header{"form-data", {{"name", "say"}, {"filename", "message.txt"}}};

    const pair<string, map<string, string>>& received_parsed_header = header_parser.parse_parameterized_header(content_disposition_header);

    REQUIRE(proper_parsed_header.first == received_parsed_header.first);
    REQUIRE(proper_parsed_header.second == received_parsed_header.second);
}