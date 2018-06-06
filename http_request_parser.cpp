#include "http_request_parser.h"

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
        string shrinked_url = url.substr(url.find('?') + 1);;
        return shrinked_url;
    }

    void http_request_parser::parse_request_parameters(const string& request_parameters, http_request& request) {
        bool parameter_name_appeared = true;

        string parameter_name;
        string parameter_value;

        char request_parameters_delimiter = '&';

        for (char current_char : request_parameters) {
            if (current_char == request_parameters_delimiter) {
                parameter_name_appeared = true;

                request_param current_request_parameter;

                current_request_parameter.name = parameter_name;
                current_request_parameter.value = parameter_value;

                request.add_http_request_param(current_request_parameter);

                parameter_name.clear();
                parameter_value.clear();

                continue;
            }

            if (current_char == '=') {
                parameter_name_appeared = false;
                continue;
            }

            if (parameter_name_appeared) {
                parameter_name.push_back(current_char);
            }
            else {
                parameter_value.push_back(current_char);
            }
        }

        request_param current_request_parameter;

        current_request_parameter.name = parameter_name;
        current_request_parameter.value = parameter_value;

        request.add_http_request_param(current_request_parameter);
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
                break;
            }
        }

        full_url = host + resource_path;
        request.set_http_request_url(full_url);
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

    bool http_request_parser::check_if_request_url_is_absolute_path(const string& request_url) {
        return request_url[0] == '/';
    }

    http_request http_request_parser::parse_request(const vector<string> &raw_request) {
        http_request request;

        parse_request_line(request, raw_request);
        parse_url_to_parameters(request);
        parse_headers(request, raw_request);

        if (check_if_request_url_is_absolute_path) {
            extend_request_url_by_host(request);
        }

        if (request.get_request_method() == "POST") {
            post_parser.parse_post_request(request, raw_request);
        }

        return request;
    }

    http_request_parser::ContentType http_request_parser::parse_content_type_header(const http_header& content_type_header) {
        ContentType contentType;

        string content_type = content_type_header.value;

        size_t index = content_type.find(';');

        string type = (index != string::npos) ? content_type.substr(0, index) : content_type;

        contentType.type = type;

        //parse parameters
        if (index != string::npos) {
            string key;
            string value;
            string parameter;

            char key_value_delimiter = '=';

            size_t begin_of_parameter = index;
            size_t end_of_parameter;

            while ((index = content_type.find(';', index)) != string::npos) {

                end_of_parameter = index;
                parameter = content_type.substr(begin_of_parameter, end_of_parameter);

                key = parameter.substr(0, parameter.find(key_value_delimiter));
                value = parameter.substr(parameter.find(key_value_delimiter) + 1);

                //remove quotes
                if (value[0] == '"') {
                    value = value.substr(1, value.length() - 2);
                }

                contentType.parameters.emplace(key, value);

                begin_of_parameter = end_of_parameter + 1;
            }

            parameter = content_type.substr(begin_of_parameter);
            key = parameter.substr(0, parameter.find(key_value_delimiter));
            value = parameter.substr(parameter.find(key_value_delimiter));

            contentType.parameters.emplace(key, value);
        }

        return contentType;
    }
}
