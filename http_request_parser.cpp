#include <regex>
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
                    size_t value_end_position = current_message_line->size() - 1;
                    current_header_value = current_message_line->substr(value_start_position, value_end_position - value_start_position - 1);
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

    vector<string> http_request_parser::get_raw_request_body(const vector<string> &raw_request) {
        vector<string> raw_request_body;
        string body;

        unsigned long index_start_of_body;
        for (unsigned long current_request_line_number = 0; current_request_line_number < raw_request.size(); current_request_line_number++) {
            if (raw_request[current_request_line_number] == "\r\n") {
                index_start_of_body = current_request_line_number + 1;
                break;
            }
        }

        body = raw_request[index_start_of_body];
        //add "\r\n" to the end for correct spliting the body
        body += "\r\n";

        string token;
        istringstream iss(body);

        regex rx("[^\r\n]+\r\n");
        sregex_iterator formated_body_list(body.begin(), body.end(), rx), rxend;

        while(formated_body_list != rxend) {
            const string& current_line = formated_body_list->str();
            raw_request_body.emplace_back(current_line.substr(0, current_line.length() - 2));
            ++formated_body_list;
        }

        return raw_request_body;
    }

    void http_request_parser::parse_urlencoded_body(http_request &post_request, const vector<string> &raw_request_body) {
        char tuples_delimiter = '&';
        char key_value_delimiter = '=';

        string key;
        string value;

        bool key_appeared = true;

        for (const auto& current_line : raw_request_body) {
            for (const auto& current_char : current_line) {
                if (current_char == tuples_delimiter) {
                    key_appeared = true;

                    post_request.add_request_body_field(key, value);

                    key.clear();
                    value.clear();

                    continue;
                }

                if (current_char == key_value_delimiter) {
                    key_appeared = false;
                    continue;
                }

                key_appeared ? key.push_back(current_char) :  value.push_back(current_char);
            }
        }

        post_request.add_request_body_field(key, value);
    }



    bool http_request_parser::check_if_current_request_body_line_is_end_boundary(const string &line, const string &boundary) {
        return line.find("--" + boundary + "--") == 0;
    }

    bool http_request_parser::check_if_current_request_body_line_is_boundary(const string &line, const string& boundary) {
        return line.find("--" + boundary) == 0;
    }

    void http_request_parser::parse_formdata_body(http_request &post_request, const vector<string> &raw_request_body, const string& boundary) {
        string key;
        string value;

        for (auto current_line = raw_request_body.begin(); current_line != raw_request_body.end(); current_line++) {
            if (check_if_current_request_body_line_is_end_boundary(*current_line, boundary)) {
                break;
            }

            if (check_if_current_request_body_line_is_boundary(*current_line, boundary)) {
                current_line++;
                content_disposition current_body_subpart_content_disposition = parse_content_disposition_header(*current_line);

                key = current_body_subpart_content_disposition.parameters["name"];

                current_line++;

                while(*current_line == "\r\n") {
                    current_line++;
                }

                while (!check_if_current_request_body_line_is_boundary(*current_line, boundary)) {
                    value += *current_line;

                    if (current_line + 1 != raw_request_body.end() && !check_if_current_request_body_line_is_boundary(*(current_line + 1), boundary)) {
                        current_line++;
                    }
                    else {
                        break;
                    }
                }

                post_request.add_request_body_field(key, value);

                key.clear();
                value.clear();
            }
        }
    }

    void http_request_parser::parse_post_request(http_request& post_request, const vector<string> &raw_request) {
        const vector<string>& raw_request_body = get_raw_request_body(raw_request);

        content_type obj = parse_content_type_header(post_request.get_header("Content-Type").value);

        if (obj.type == "application/x-www-form-urlencoded") {
            parse_urlencoded_body(post_request, raw_request_body);
        }
        else {
            parse_formdata_body(post_request, raw_request_body, obj.parameters["boundary"]);
        }
    }

    http_request http_request_parser::parse_request(const vector<string> &raw_request) {
        http_request request;

        parse_request_line(request, raw_request);
        parse_url_to_parameters(request);
        parse_headers(request, raw_request);

        const string& url = request.get_request_url();
        if (check_if_request_url_is_absolute_path(url)) {
            extend_request_url_by_host(request);
        }

        if (request.get_request_method() == "POST") {
            parse_post_request(request, raw_request);
        }

        return request;
    }

    http_request_parser::content_type http_request_parser::parse_content_type_header(const string& content_type_header) {
        content_type obj;

        size_t index = content_type_header.find(';');

        string type = (index != string::npos) ? content_type_header.substr(0, index) : content_type_header;

        obj.type = type;

        //parse parameters
        if (index != string::npos) {
            string key;
            string value;
            string parameter;

            char key_value_delimiter = '=';

            size_t begin_of_parameter = index + 1;
            size_t end_of_parameter;

            while(content_type_header[begin_of_parameter] == ' ') {
                begin_of_parameter++;
            }

            while ((index = content_type_header.find(';', index + 1)) != string::npos) {
                end_of_parameter = index;
                parameter = content_type_header.substr(begin_of_parameter, end_of_parameter - begin_of_parameter);

                key = parameter.substr(0, parameter.find(key_value_delimiter));
                value = parameter.substr(parameter.find(key_value_delimiter) + 1);

                //remove quotes
                if (value[0] == '"') {
                    value = value.substr(1, value.length() - 2);
                }

                obj.parameters.emplace(key, value);

                begin_of_parameter = end_of_parameter + 1;
                while(content_type_header[begin_of_parameter] == ' ') {
                    begin_of_parameter++;
                }
            }

            parameter = content_type_header.substr(begin_of_parameter);
            key = parameter.substr(0, parameter.find(key_value_delimiter));
            value = parameter.substr(parameter.find(key_value_delimiter) + 1);

            if (value[0] == '"') {
                value = value.substr(1, value.length() - 2);
            }

            obj.parameters.emplace(key, value);
        }

        return obj;
    }

    http_request_parser::content_disposition http_request_parser::parse_content_disposition_header(const string& raw_content_disposition_header) {
        content_disposition obj;

        size_t index = raw_content_disposition_header.find(';');

        string type = (index != string::npos) ? raw_content_disposition_header.substr(0, index) : raw_content_disposition_header;

        obj.type = type;

        //parse parameters
        if (index != string::npos) {
            string key;
            string value;
            string parameter;

            char key_value_delimiter = '=';

            size_t begin_of_parameter = index + 1;
            size_t end_of_parameter;

            while (raw_content_disposition_header[begin_of_parameter] == ' ') {
                begin_of_parameter++;
            }

            while ((index = raw_content_disposition_header.find(';', index + 1)) != string::npos) {
                end_of_parameter = index;
                parameter = raw_content_disposition_header.substr(begin_of_parameter, end_of_parameter - begin_of_parameter);

                key = parameter.substr(0, parameter.find(key_value_delimiter));
                value = parameter.substr(parameter.find(key_value_delimiter) + 1);

                //remove quotes
                if (value[0] == '"') {
                    value = value.substr(1, value.length() - 2);
                }

                obj.parameters.emplace(key, value);

                begin_of_parameter = end_of_parameter + 1;
                while (raw_content_disposition_header[begin_of_parameter] == ' ') {
                    begin_of_parameter++;
                }
            }

            parameter = raw_content_disposition_header.substr(begin_of_parameter);
            key = parameter.substr(0, parameter.find(key_value_delimiter));
            value = parameter.substr(parameter.find(key_value_delimiter) + 1);

            if (value[0] == '"') {
                value = value.substr(1, value.length() - 2);
            }

            obj.parameters.emplace(key, value);
        }

        return obj;
    }
}
