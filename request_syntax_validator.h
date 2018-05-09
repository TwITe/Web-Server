#ifndef WEB_REQUEST_SYNTAX_VALIDATOR_H
#define WEB_REQUEST_SYNTAX_VALIDATOR_H

#include "http_request.h"

namespace webserver {
    class request_syntax_validator {
    private:
        bool check_request_line_method(const string &request_method);
        bool check_request_line_http_version(const string &request_http_version);
        bool check_request_line_url(const string& request_url, const http_request& request);
        bool check_request_line(const http_request& request);

        set<string> available_request_methods;
    public:
        request_syntax_validator();
        bool check_request(const http_request& request, const vector<string>& raw_request);
    };
}

#endif //WEB_REQUEST_SYNTAX_VALIDATOR_H