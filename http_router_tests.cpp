#include "web_server.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("GetProperWebHandler_RequestPOSTW/oParametersW/Path_ReceivedPostRequestHandler", "HTTP Router") {
    vector<webserver::web_handler> handlers;

    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
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
    _Pragma("GCC diagnostic pop")

    webserver::web_handler proper_web_handler("/im", "POST", index_handler);

    handlers.emplace_back(proper_web_handler);

    webserver::http_request test_request;
    test_request.set_http_request_method("POST");
    test_request.set_http_request_url("https://vk.com/im");

    webserver::http_router test_router;
    webserver::web_handler received_web_handler = test_router.get_suitable_request_handler(handlers, test_request);

    const string& received_web_handler_method = received_web_handler.get_web_handler_method();
    const string& received_web_handler_pattern = received_web_handler.get_web_handler_pattern();

    const string& proper_web_handler_method = proper_web_handler.get_web_handler_method();
    const string& proper_web_handler_pattern = proper_web_handler.get_web_handler_pattern();

    REQUIRE(received_web_handler_method == proper_web_handler_method);
    REQUIRE(received_web_handler_pattern == proper_web_handler_pattern);
}

TEST_CASE("GetProperWebHandler_RequestPOSTW/ParametersW/Path_ReceivedPostRequestHandler", "HTTP Router") {
    vector<webserver::web_handler> handlers;

    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
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
    _Pragma("GCC diagnostic pop")

    webserver::web_handler proper_web_handler("/feed", "POST", index_handler);

    handlers.emplace_back(proper_web_handler);

    webserver::http_request test_request;
    test_request.set_http_request_method("POST");
    test_request.set_http_request_url("https://vk.com/feed?section=live");

    webserver::http_router test_router;
    webserver::web_handler received_web_handler = test_router.get_suitable_request_handler(handlers, test_request);

    const string& received_web_handler_method = received_web_handler.get_web_handler_method();
    const string& received_web_handler_pattern = received_web_handler.get_web_handler_pattern();

    const string& proper_web_handler_method = proper_web_handler.get_web_handler_method();
    const string& proper_web_handler_pattern = proper_web_handler.get_web_handler_pattern();

    REQUIRE(received_web_handler_method == proper_web_handler_method);
    REQUIRE(received_web_handler_pattern == proper_web_handler_pattern);
}

TEST_CASE("GetProperWebHandler_RequestPOSTW/oParametersW/oPath_ReceivedPostRequestHandler", "HTTP Router") {
    vector<webserver::web_handler> handlers;

    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
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
    _Pragma("GCC diagnostic pop")

    webserver::web_handler proper_web_handler("/", "POST", index_handler);

    handlers.emplace_back(proper_web_handler);

    webserver::http_request test_request;
    test_request.set_http_request_method("POST");
    test_request.set_http_request_url("https://vk.com");

    webserver::http_router test_router;
    webserver::web_handler received_web_handler = test_router.get_suitable_request_handler(handlers, test_request);

    const string& received_web_handler_method = received_web_handler.get_web_handler_method();
    const string& received_web_handler_pattern = received_web_handler.get_web_handler_pattern();

    const string& proper_web_handler_method = proper_web_handler.get_web_handler_method();
    const string& proper_web_handler_pattern = proper_web_handler.get_web_handler_pattern();

    REQUIRE(received_web_handler_method == proper_web_handler_method);
    REQUIRE(received_web_handler_pattern == proper_web_handler_pattern);
}

TEST_CASE("GetProperWebHandler_RequestPOSTW/ParametersW/oPath_ReceivedPostRequestHandler", "HTTP Router") {
    vector<webserver::web_handler> handlers;

    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
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
    _Pragma("GCC diagnostic pop")

    webserver::web_handler proper_web_handler("/", "POST", index_handler);

    handlers.emplace_back(proper_web_handler);

    webserver::http_request test_request;
    test_request.set_http_request_method("POST");
    test_request.set_http_request_url("https://www.yandex.ru/?clid=2063711");

    webserver::http_router test_router;
    webserver::web_handler received_web_handler = test_router.get_suitable_request_handler(handlers, test_request);

    const string& received_web_handler_method = received_web_handler.get_web_handler_method();
    const string& received_web_handler_pattern = received_web_handler.get_web_handler_pattern();

    const string& proper_web_handler_method = proper_web_handler.get_web_handler_method();
    const string& proper_web_handler_pattern = proper_web_handler.get_web_handler_pattern();

    REQUIRE(received_web_handler_method == proper_web_handler_method);
    REQUIRE(received_web_handler_pattern == proper_web_handler_pattern);
}

TEST_CASE("NonexistentProperWebHandler_RequestGET_ReceiveErrorHandler", "HTTP Router") {
    vector<webserver::web_handler> handlers;

    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
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
    _Pragma("GCC diagnostic pop")

    webserver::web_handler proper_web_handler("/im", "POST", index_handler);

    handlers.emplace_back(proper_web_handler);

    webserver::http_request test_request;
    test_request.set_http_request_method("GET");
    test_request.set_http_request_url("https://vk.com/im");

    webserver::http_router test_router;

    webserver::web_handler received_web_handler = test_router.get_suitable_request_handler(handlers, test_request);

    const string& received_web_handler_method = received_web_handler.get_web_handler_method();
    const string& received_web_handler_pattern = received_web_handler.get_web_handler_pattern();

    const function<webserver::http_response(webserver::http_request)>& handler = received_web_handler.get_transform_to_response_function();
    webserver::http_response received_response = handler(test_request);
    const string& response_body = received_response.get_response_body();

    REQUIRE(received_web_handler_method.empty());
    REQUIRE(received_web_handler_pattern.empty());
    REQUIRE(response_body == "Not Found");
}