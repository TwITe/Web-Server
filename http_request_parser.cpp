#include "http_request_parser.h"
#include <sstream>
using namespace std;

namespace webserver {
    void http_request_parser::parse_request_line(http_request& request, const vector<string>& raw_request) {
        istringstream request_line(raw_request[0]);

        string method;
        string request_url;
        string protocol_version;

        request_line >> method >> request_url >> protocol_version;

        request.set_http_request_method(method);
        request.set_http_request_url(request_url);
        request.set_http_version(protocol_version);
    }

    bool http_request_parser::check_if_request_parameters_exists(const string& url) {
        for (auto current_char : url) {
            if (current_char == '?') {
                return true;
            }
        }
        return false;
    }

    string http_request_parser::shrink_url_to_parameters(const string& url) {
        string shrinked_url = url.substr(url.find('?'));;
        return shrinked_url;
    }

    void http_request_parser::parse_request_parameters(const string& request_parameters, http_request& request) {
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

        if (check_if_request_parameters_exists(url)) {
            url = shrink_url_to_parameters(url);
            parse_request_parameters(url, request);
        }
    }

    void http_request_parser::extend_request_url_by_host(http_request& request) {
        string full_url;
        string host;
        string resource_path = request.get_request_url();

        const vector<http_header> headers = request.get_headers();
        for (auto current_header: headers) {
            if (current_header.type == "Host") {
                host = current_header.value;
            }
        }

        full_url = host + resource_path;
        request.set_http_request_url(full_url);
    }

    void http_request_parser::parse_request_body(http_request& request, const vector<string>& raw_http_request) {
        for (auto current_message_line = raw_http_request.begin(); current_message_line != raw_http_request.end(); current_message_line++) {
            if (*current_message_line == "\r\n" && current_message_line != raw_http_request.end()) {
                request.set_http_request_body(*(current_message_line + 1));
            }
        }
    }

    void http_request_parser::parse_headers(http_request& request, const vector<string>& raw_http_request) {
        string headers_and_body_delimiter = "\r\n";
        char header_type_and_value_delimiter = ':';

        //парсинг начинается со второй строчки, где и начинаются хэдеры
        for (auto current_message_line = raw_http_request.begin() + 1; *current_message_line != headers_and_body_delimiter; current_message_line++) {

            string current_header_type;
            string current_header_value;

            for (size_t current_char_postion = 0; current_char_postion < current_message_line->size(); current_char_postion++) {
                if ((*current_message_line)[current_char_postion] != header_type_and_value_delimiter) {
                    current_header_type.push_back((*current_message_line)[current_char_postion]);
                }
                else {
                    size_t value_start_position = current_char_postion + 2;
                    current_header_value = current_message_line->substr(value_start_position);
                    break;
                }
            }

            http_header current_header;
            current_header.type = current_header_type;
            current_header.value = current_header_value;
            request.add_http_request_header(current_header);
        }
    }

    http_request http_request_parser::parse_request(vector<string> &raw_request) {
        http_request request;

        parse_request_line(request, raw_request);
        parse_url_to_parameters(request);
        parse_headers(request, raw_request);
        parse_request_body(request, raw_request);

        string request_body = request.get_request_body();

        const string& request_url = request.get_request_url();
        if (request_url[0] == '/') {
            extend_request_url_by_host(request);
        }

        return request;
    }
}
