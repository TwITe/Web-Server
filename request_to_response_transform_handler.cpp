#include "request_to_response_transform_handler.h"

namespace webserver {
    http_response request_to_response_transform_handler::transform_request_to_response(const web_handler& handler, const http_request& request) {
        const function<http_response(http_request)>& request_transform_fuction = handler.get_transform_to_response_function();
        return request_transform_fuction(request);
    }
}
