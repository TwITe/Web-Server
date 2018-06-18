#include <cpr/cpr.h>
#include <iostream>
#include "web_server.h"
#include <vector>
#include <thread>
#include "catch.hpp"
using namespace cpr;

const unsigned short int PORT = 8080;
webserver::web_server* server;

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

vector<webserver::web_handler> handlers{get_web_handler, post_reflect_web_handler, health_check_handler};

TEST_CASE("Run server", "[Component Tests][Health Check Tests]") {
    server = new webserver::web_server(PORT, handlers);

    thread server_run = thread([&]{server->start();});
    server_run.detach();

    while (Get(cpr::Url("http://localhost:8080/is_server_up")).error.code != cpr::ErrorCode::OK) {}
}

TEST_CASE("Resource Not Found Test", "[Component Tests][Get Request Tests]") {
    std::string expected_body_text = "Not Found";

    auto received_response = Get(cpr::Url{"http://localhost:8080/error_url"});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(404 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE(to_string(expected_body_text.length()) == received_response.header["Content-Length"]);
}

TEST_CASE("Resource Found Test", "[Component Tests][Get Request Tests]") {
    std::string expected_body_text = "Hello, World!";

    auto received_response = Get(cpr::Url{"http://localhost:8080/index_get"});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(200 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE(to_string(expected_body_text.length()) == received_response.header["Content-Length"]);
}

TEST_CASE("Payload Single parameter", "[Component Tests][UrlEncoded Post Tests]") {
    string expected_body_text = "say: Hi";

    auto received_response = Post(cpr::Url{"http://localhost:8080/reflect_message"}, cpr::Payload{{"say", "Hi"}});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(200 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE(to_string(expected_body_text.length()) == received_response.header["Content-Length"]);
}

TEST_CASE("Payload Multiple parameters", "[Component Tests][UrlEncoded Post Tests]") {
    string expected_body_text = "say: Hi\r\n"
                                "to: mom";

    auto received_response = Post(cpr::Url{"http://localhost:8080/reflect_message"}, cpr::Payload{{"say", "Hi"}, {"to", "mom"}});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(200 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE(to_string(expected_body_text.length()) == received_response.header["Content-Length"]);
}

TEST_CASE("Multipart Single Subpart", "[Component Tests][FormData Post Tests]") {
    string expected_body_text = "say: Hi";

    auto received_response = Post(cpr::Url{"http://localhost:8080/reflect_message"}, cpr::Multipart{{"say", "Hi"}});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(200 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE(to_string(expected_body_text.length()) == received_response.header["Content-Length"]);
}

TEST_CASE("Multipart Multiple Subparts", "[Component Tests][FormData Post Tests]") {
    string expected_body_text = "say: Hi\r\n"
                                "to: mom";

    auto received_response = Post(cpr::Url{"http://localhost:8080/reflect_message"}, cpr::Multipart{{"say", "Hi"}, {"to", "mom"}});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(200 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE(to_string(expected_body_text.length()) == received_response.header["Content-Length"]);
}

TEST_CASE("Shut Down The Server", "[Server Shutdown") {
    server->stop();

    auto r = Get(cpr::Url("http://localhost:8080/is_server_up"), cpr::Timeout(1000));

    cout << r.error.message << endl;

    REQUIRE(r.error.code == cpr::ErrorCode::OPERATION_TIMEDOUT);

    delete server;

    server = new webserver::web_server(PORT, handlers);

    thread server_run = thread([&]{server->start();});
    server_run.detach();

    while (Get(cpr::Url("http://localhost:8080/is_server_up")).error.code != cpr::ErrorCode::OK) {}

    REQUIRE(Get(cpr::Url("http://localhost:8080/is_server_up")).status_code == 200);
}