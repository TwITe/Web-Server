#include <cpr/cpr.h>
#include <iostream>
#include "catch.hpp"
#include "web_server.h"
#include <vector>
#include <thread>
using namespace cpr;

const unsigned short int PORT = 8080;

_Pragma("GCC diagnostic push")
_Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
function<webserver::http_response(webserver::http_request)> transform_get_request = [&](webserver::http_request request) {
    webserver::http_response response;

    string response_body = "Hello, World!";

    response.set_response_body(response_body);
    response.set_response_http_code(200);
    response.set_response_length(response_body.size());

    vector<webserver::http_header> headers;
    headers.push_back(webserver::http_header{"Content-Type", "text/plain"});
    response.set_response_headers(headers);

    return response;
};
_Pragma("GCC diagnostic pop")

webserver::web_handler get_web_handler("/index_get", "GET", transform_get_request);

_Pragma("GCC diagnostic push")
_Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
function<webserver::http_response(webserver::http_request)> transform_post_request_reflect_message = [&](webserver::http_request request) {
    webserver::http_response response;

    const map<string, string>& request_body = request.get_request_body();

    string response_body;

    for (const auto& current_tuple : request_body) {
        response_body += current_tuple.first + ": " + current_tuple.second + "\r\n";
    }

    response_body.pop_back();
    response_body.pop_back();

    response.set_response_body(response_body);
    response.set_response_http_code(200);
    response.set_response_length(response_body.size());

    vector<webserver::http_header> headers;

    headers.push_back(webserver::http_header{"Content-Type", "text/plain"});

    response.set_response_headers(headers);

    return response;
};
_Pragma("GCC diagnostic pop")

webserver::web_handler post_reflect_web_handler("/reflect_message", "POST", transform_post_request_reflect_message);

vector<webserver::web_handler> handlers{get_web_handler, post_reflect_web_handler};

webserver::web_server server(PORT, handlers);


TEST_CASE("Resource Not Found Test", "Get Request Tests") {
    thread server_run([]{server.start();});
    server_run.detach();

    std::string expected_body_text = "Not Found";

    auto received_response = Get(cpr::Url{"http://localhost:8080/error_url"});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(404 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE(to_string(expected_body_text.length()) == received_response.header["Content-Length"]);
}

TEST_CASE("Resource Found Test", "Get Request Tests") {
    std::string expected_body_text = "Hello, World!";

    auto received_response = Get(cpr::Url{"http://localhost:8080/index_get"});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(200 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE(to_string(expected_body_text.length()) == received_response.header["Content-Length"]);
}

TEST_CASE("Payload Single parameter", "UrlEncoded Post Tests") {
    string expected_body_text = "say: Hi";

    auto received_response = Post(cpr::Url{"http://localhost:8080/reflect_message"}, cpr::Payload{{"say", "Hi"}});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(200 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE(to_string(expected_body_text.length()) == received_response.header["Content-Length"]);
}

TEST_CASE("Payload Multiple parameters", "UrlEncoded Post Tests") {
    string expected_body_text = "say: Hi\r\n"
                                "to: mom";

    auto received_response = Post(cpr::Url{"http://localhost:8080/reflect_message"}, cpr::Payload{{"say", "Hi"}, {"to", "mom"}});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(200 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE(to_string(expected_body_text.length()) == received_response.header["Content-Length"]);
}

TEST_CASE("Multipart Single Subpart", "FormData Post Tests") {
    string expected_body_text = "say: Hi";

    auto received_response = Post(cpr::Url{"http://localhost:8080/reflect_message"}, cpr::Multipart{{"say", "Hi"}});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(200 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE(to_string(expected_body_text.length()) == received_response.header["Content-Length"]);
}

TEST_CASE("Multipart Multiple Subparts", "FormData Post Tests") {
    string expected_body_text = "say: Hi\r\n"
                                "to: mom";

    auto received_response = Post(cpr::Url{"http://localhost:8080/reflect_message"}, cpr::Multipart{{"say", "Hi"}, {"to", "mom"}});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(200 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE(to_string(expected_body_text.length()) == received_response.header["Content-Length"]);
}