#include "http_parameterized_header_parser.h"

namespace webserver {
    void http_request_parameterized_header_parser::set_parameterized_header_type(
            pair<string, map<string, string>>& parsed_parameterized_header, const string& type) {

        parsed_parameterized_header.first = type;
    }

    void http_request_parameterized_header_parser::add_parameterized_header_parameter(
            pair<string, map<string, string>>& parsed_parameterized_header, const string& key, const string& value) {

        parsed_parameterized_header.second.emplace(key, value);
    }

    pair<string, map<string, string>> http_request_parameterized_header_parser::parse_parameterized_header(const string& raw_parameterized_header) {
        pair<string, map<string, string>> parsed_parameterized_header;

        size_t index = raw_parameterized_header.find(';');

        string type = (index != string::npos) ? raw_parameterized_header.substr(0, index) : raw_parameterized_header;

        set_parameterized_header_type(parsed_parameterized_header, type);

        //parse parameters
        if (index != string::npos) {
            string key;
            string value;
            string parameter;

            char key_value_delimiter = '=';

            size_t begin_of_parameter = index + 1;
            size_t end_of_parameter;

            while(raw_parameterized_header[begin_of_parameter] == ' ') {
                begin_of_parameter++;
            }

            while ((index = raw_parameterized_header.find(';', index + 1)) != string::npos) {
                end_of_parameter = index;
                parameter = raw_parameterized_header.substr(begin_of_parameter, end_of_parameter - begin_of_parameter);

                key = parameter.substr(0, parameter.find(key_value_delimiter));
                value = parameter.substr(parameter.find(key_value_delimiter) + 1);

                //remove quotes
                if (value[0] == '"') {
                    value = value.substr(1, value.length() - 2);
                }

                add_parameterized_header_parameter(parsed_parameterized_header, key, value);

                begin_of_parameter = end_of_parameter + 1;
                while(raw_parameterized_header[begin_of_parameter] == ' ') {
                    begin_of_parameter++;
                }
            }

            parameter = raw_parameterized_header.substr(begin_of_parameter);
            key = parameter.substr(0, parameter.find(key_value_delimiter));
            value = parameter.substr(parameter.find(key_value_delimiter) + 1);

            if (value[0] == '"') {
                value = value.substr(1, value.length() - 2);
            }

            add_parameterized_header_parameter(parsed_parameterized_header, key, value);
        }

        return parsed_parameterized_header;
    }
}