#include "http_request_parser.h"
#include <sstream>
using namespace std;

namespace webserver {

    bool http_request_parser::check_is_current_header_host(const string& current_header) {
        return current_header == "Host";
    }

    void http_request_parser::parse_request_line(http_request& request, const vector<string>& raw_http_request) {
        istringstream request_line(raw_http_request[0]);

        string method;
        string request_url;

        request_line >> method >> request_url;

        request.set_http_request_method(method);
        request.set_http_request_url(request_url);
    }

    void http_request_parser::extend_request_url_by_host(const string& host, http_request& request) {
        string full_url;
        string url_second_part = request.get_request_url();

        full_url = host + url_second_part;

        request.set_http_request_url(full_url);
    }

    void http_request_parser::parse_request_body(http_request& request, const vector<string>& raw_http_request) {
        for (auto current_message_line = raw_http_request.begin();
             current_message_line != raw_http_request.end(); current_message_line++) {
            if (*current_message_line == " ") {
                request.set_http_request_body(*(current_message_line + 1));
            }
        }
    }

    void http_request_parser::parse_headers(http_request& request, const vector<string>& raw_http_request) {
        for (auto current_message_line = raw_http_request.begin() + 1;
             current_message_line != raw_http_request.end(); current_message_line++) {
            string current_header_type;
            string current_header_value;

            if (*current_message_line == " ") {
                break;
            }

            for (size_t current_char_postion = 0; current_char_postion < current_message_line->size(); current_char_postion++) {
                if ((*current_message_line)[current_char_postion] == ':') {
                    current_header_value = current_message_line->substr(current_char_postion + 2);
                    break;
                }
                current_header_type.push_back((*current_message_line)[current_char_postion]);
            }

            http_header current_header;
            current_header.type = current_header_type;
            current_header.value = current_header_value;

            request.add_http_request_header(current_header);

            if (check_is_current_header_host(current_header_type)) {
                extend_request_url_by_host(current_header_value, request);
            }
        }
    }

    http_request http_request_parser::parse(vector<string>& raw_http_request) {
        http_request request;

        parse_request_line(request, raw_http_request);
        parse_headers(request, raw_http_request);
        if (request.check_is_request_body_exist()) {
            parse_request_body(request, raw_http_request);
        }

        return request;
    }
}
