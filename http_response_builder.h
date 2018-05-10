//
// Created by TwITe on 05/09/2018.
//

#ifndef WEB_HTTP_RESPONSE_BUILDER_H
#define WEB_HTTP_RESPONSE_BUILDER_H

#include "http_response.h"
#include "web_handler.h"
#include <map>

namespace webserver {
    class http_response_builder {
    private:
        map<int, string> reason_phrases;
    public:
        http_response_builder();

        string build_response(const web_handler& request_handler, const http_request& request);
    };
}


#endif //WEB_HTTP_RESPONSE_BUILDER_H
