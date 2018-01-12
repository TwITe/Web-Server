#include "web_server.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("Get Proper Suitable Web Handler", "HTTP Router") {
    vector<webserver::web_handler> handlers;

    function<webserver::http_response(webserver::http_request)> index_handler = [&](webserver::http_request request) {
        webserver::http_response response;

        string response_body = "hello world!";

        response.set_response_body(response_body);
        response.set_response_http_code(200);
        response.set_response_length(response_body.size());

        vector<webserver::http_header> headers;
        headers.push_back(webserver::http_header{"Content-Type", "text/plain"});
        response.set_response_headers(headers);

        return response;
    };
    webserver::web_handler proper_web_handler("/im", "POST", index_handler);

    handlers.emplace_back(proper_web_handler);

    webserver::http_request test_request;
    test_request.set_http_request_method("POST");
    test_request.set_http_request_url("https://vk.com/im");

    webserver::http_router test_router;
    webserver::web_handler received_web_handler = test_router.get_suitable_request_handler(handlers, test_request);


    const string& received_web_handler_method = received_web_handler.get_web_handler_method(), proper_web_handler_method = proper_web_handler.get_web_handler_method();
    const string& received_web_handler_pattern = received_web_handler.get_web_handler_pattern(), proper_web_handler_pattern = proper_web_handler.get_web_handler_pattern();

    REQUIRE(received_web_handler_method == proper_web_handler_method);
    REQUIRE(received_web_handler_pattern == proper_web_handler_pattern);
}

TEST_CASE("Non-existing Handler: Get Error Handler", "HTTP Router") {
    vector<webserver::web_handler> handlers;

    function<webserver::http_response(webserver::http_request)> index_handler = [&](webserver::http_request request) {
        webserver::http_response response;

        string response_body = "hello world!";

        response.set_response_body(response_body);
        response.set_response_http_code(200);
        response.set_response_length(response_body.size());

        vector<webserver::http_header> headers;
        headers.push_back(webserver::http_header{"Content-Type", "text/plain"});
        response.set_response_headers(headers);

        return response;
    };
    webserver::web_handler proper_web_handler("/im", "POST", index_handler);

    handlers.emplace_back(proper_web_handler);

    webserver::http_request test_request;
    test_request.set_http_request_method("GET");
    test_request.set_http_request_url("https://vk.com/im");

    webserver::http_router test_router;
    webserver::web_handler received_web_handler = test_router.get_suitable_request_handler(handlers, test_request);

    const string& received_web_handler_method = received_web_handler.get_web_handler_method();
    const string& received_web_handler_pattern = received_web_handler.get_web_handler_pattern();

    webserver::http_response transformed_response = received_web_handler.transform_request_to_response(test_request);
    const string& incorrect_response_body = transformed_response.get_response_body();

    REQUIRE(received_web_handler_method.empty());
    REQUIRE(received_web_handler_pattern.empty());
    REQUIRE(incorrect_response_body == "Oops! Something went wrong");
}