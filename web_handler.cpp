#include "web_handler.h"

namespace webserver {
    web_handler::web_handler(string pattern, string method, function<http_response(http_request)> handler) :
            pattern(pattern), method(method), handler(handler) {};

    const string& web_handler::get_web_handler_method() {
        return method;
    }

    const string& web_handler::get_web_handler_pattern() {
        return pattern;
    }

    http_response web_handler::transform_request_to_response(const http_request& received_request) {
        return handler(received_request);
    }
}