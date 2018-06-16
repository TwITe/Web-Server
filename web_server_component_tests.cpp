#include <cpr/cpr.h>
#include <iostream>
#include "catch.hpp"
//#include "web_server.h"
using namespace cpr;

TEST_CASE("Resource Not Found Test", "Get Request Tests") {
    std::string expected_body_text = "Not Found";

    auto received_response = Get(cpr::Url{"http://localhost:8080/error"});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(404 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE("9" == received_response.header["Content-Length"]);
}

TEST_CASE("Resource Found Test", "Get Request Tests") {
    std::string expected_body_text = "Hello, World!";

    auto received_response = Get(cpr::Url{"http://localhost:8080/index_get"});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(200 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE("13" == received_response.header["Content-Length"]);
}

TEST_CASE("Post Plain Text Body", "Post Request Tests") {
    std::string expected_body_text = "Hello, World!";

    auto received_response = Post(cpr::Url{"http://localhost:8080/index_post"}, cpr::Body{"Hello, World!"});

    REQUIRE(expected_body_text == received_response.text);
    REQUIRE(201 == received_response.status_code);
    REQUIRE("text/plain" == received_response.header["Content-Type"]);
    REQUIRE("13" == received_response.header["Content-Length"]);
    REQUIRE("http://localhost:8080/new_created_page" == received_response.header["Location"]);
}

//TEST_CASE("UrlEncoded Body", "Post Request Tests") {
//
//}