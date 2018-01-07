#include "web_handler.h"

namespace webserver {
    web_handler::web_handler(string pattern, string method, function<http_response(http_request)> handler) :
            pattern{pattern}, method{method}, handler{handler} {}
}