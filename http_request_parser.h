#ifndef WEB_HTTP_REQUEST_PARSER_H
#define WEB_HTTP_REQUEST_PARSER_H

#include <iostream>
#include <vector>
#include <functional>
#include <thread>
#include <sstream>
#include "http_request.h"

namespace webserver {
    class http_request_parser {
    private:
        bool check_is_current_header_host(const string& current_header) {}

        void parse_request_line(http_request& request, const vector<string>& raw_http_request) {}

        void extend_request_url_by_host(const string& host, http_request& request) {}

        void parse_request_body(http_request& request, const vector<string>& raw_http_request) {}

        void parse_headers(http_request& request, const vector<string>& raw_http_request) {}

    public:
        http_request parse(vector<string>& raw_http_request) {}
    };
}

#endif //WEB_HTTP_REQUEST_PARSER_H
