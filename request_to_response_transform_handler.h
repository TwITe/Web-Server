#ifndef WEB_REQUEST_TO_RESPONSE_TRANSFORM_HANDLER_H
#define WEB_REQUEST_TO_RESPONSE_TRANSFORM_HANDLER_H

#include "http_response.h"
#include "http_request.h"
#include "web_handler.h"
#include <functional>

namespace webserver {
    class request_to_response_transform_handler {
    public:
        http_response transform_request_to_response(const web_handler& handler, const http_request& request);
    };
}

#endif //WEB_REQUEST_TRANSFORM_HANDLER_H
