#ifndef WEB_CLIENT_HTTP_REQUEST_HANDLER_H
#define WEB_CLIENT_HTTP_REQUEST_HANDLER_H

#include "http_request_parser.h"
#include "http_response.h"
#include "http_request.h"
#include <string>
#include <vector>

namespace webserver {
    class client_http_request_handler {
    public:
        void handle_client(vector<string> client_message);
    };
}

#endif //WEB_CLIENT_HTTP_REQUEST_HANDLER_H