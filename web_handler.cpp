#include "web_handler.h"

namespace webserver {
    web_handler::web_handler(string pattern, string method, function<http_response(http_request)> handler) :
            pattern{pattern}, method{method}, handler{handler} {}

    string web_handler::get_web_handler_method() const {
        return method;
    }

    string web_handler::get_web_handler_pattern() const {
        return pattern;
    }
}