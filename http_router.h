#ifndef WEB_HTTP_ROUTER_H
#define WEB_HTTP_ROUTER_H

#include "web_handler.h"
#include "web_server.h"
#include "http_request.h"

namespace webserver {
    class http_router {
    private:
            string get_request_pattern(const http_request& request);
    public:
        vector<web_handler>::iterator get_suited_request_handler(vector<web_handler>& handlers, const http_request& request);
    };
}

#endif //WEB_HTTP_ROUTER_H
