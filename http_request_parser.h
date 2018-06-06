#ifndef WEB_HTTP_REQUEST_PARSER_H
#define WEB_HTTP_REQUEST_PARSER_H
#include "http_request.h"
#include "http_request_parser_post.h"
#include <string>
#include <sstream>

namespace webserver {
    class http_request_parser {
    private:
        void parse_request_line(http_request& request, const vector<string>& raw_request);

        string shrink_url_to_parameters(const string& url);

        void parse_request_parameters(const string& request_parameters, http_request& request);

        void parse_url_to_parameters(http_request& request);

        void extend_request_url_by_host(http_request& request);

        void parse_headers(http_request& request, const vector<string>& raw_http_request);

        bool check_if_request_parameters_exists(const string& url);

        bool check_if_request_url_is_absolute_path(const string& url);

        http_request_parser_post post_parser;
    protected:
        struct ContentType {
            string type;
            map <string, string> parameters;
        };

        ContentType parse_content_type_header(const http_header& content_type_header);
    public:
        http_request parse_request(const vector<string> &raw_request);
    };
}

#endif //WEB_HTTP_REQUEST_PARSER_H
