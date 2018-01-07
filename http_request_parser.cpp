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

    bool http_request_parser::check_is_request_parameters_exists(const string& url) {
        for (auto current_char : url) {
            if (current_char == '?') {
                return true;
            }
        }
        return false;
    }

    void http_request_parser::shrink_url_to_parameters(string& url) {
        url = url.substr(url.find('?'));
    }

    void http_request_parser::parse_parameters(const string& request_parameters, http_request& request) {
        bool parameter_name_appeared = true;

        string parameter_name;
        string parameter_value;

        for (unsigned long current_char_position = 0; current_char_position < request_parameters.size(); current_char_position++) {
            if (request_parameters[current_char_position] == '&' || current_char_position == request_parameters.size() - 1) {
                parameter_name_appeared = true;

                request_param current_request_parameter;

                current_request_parameter.name = parameter_name;
                current_request_parameter.value = parameter_value;

                request.add_http_request_param(current_request_parameter);

                parameter_name.clear();
                parameter_value.clear();

                continue;
            }

            if (request_parameters[current_char_position] == '=') {
                parameter_name_appeared = false;
                continue;
            }

            if (parameter_name_appeared) {
                parameter_name.push_back(request_parameters[current_char_position]);
            }
            else {
                parameter_value.push_back(request_parameters[current_char_position]);
            }
        }
    }

    void http_request_parser::parse_url_to_parameters(http_request& request) {
        string url = request.get_request_url();

        if (check_is_request_parameters_exists(url)) {
            shrink_url_to_parameters(url);
            parse_parameters(url, request);
        }
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

    void add_content_type_default_header(http_request& request) {
        http_header content_type_default_header;

        content_type_default_header.type = "Content-type";
        content_type_default_header.value = "utf-8";

        request.add_http_request_header(content_type_default_header);
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

        if (request.check_is_content_type_header_exists()) {
            add_content_type_default_header(request);
        }
    }

    http_request http_request_parser::parse(vector<string>& raw_http_request) {
        http_request request;

        parse_request_line(request, raw_http_request);
        parse_url_to_parameters(request);
        parse_headers(request, raw_http_request);
        if (request.check_is_content_type_header_exists()) {
            parse_request_body(request, raw_http_request);
        }

        return request;
    }
}
