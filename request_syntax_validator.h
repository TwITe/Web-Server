#ifndef WEB_REQUEST_SYNTAX_VALIDATOR_H
#define WEB_REQUEST_SYNTAX_VALIDATOR_H

#include "http_request.h"

namespace webserver {
    class request_syntax_validator {
    private:
        bool check_request_method(const string& request_method);
        bool check_request_http_version(const string& equest_http_version);
        bool check_request_line(const http_request& request);
        bool check_request_headers(const http_request& request);

        set<string> available_headers;
        set<string> available_methods;
    public:
        request_syntax_validator();
        bool check_request(const http_request& request);
    };
}

#endif //WEB_REQUEST_SYNTAX_VALIDATOR_H
