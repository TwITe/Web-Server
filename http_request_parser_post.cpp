#include "http_request_parser_post.h"

namespace webserver {
    void http_request_parser_post::detect_request_body_content_type(Request_body *post_request_body, const ContentType &content_type) {
        if (content_type.type == "application/x-www-form-urlencoded") {
            post_request_body = new UrlencodedBody;
        }
        else {
            post_request_body = new FormDataBody;
        }
    }

    void http_request_parser_post::UrlencodedBody::parse_request_body(http_request &post_request, const vector<string> &raw_request_body, const ContentType &obj) {

        char key_value_tuples_delimiter = '&';
        char key_value_delimiter = '=';

        string key;
        string value;

        bool key_appeared = true;

        for (const auto& current_line : raw_request_body) {
            for (const auto& current_char : current_line) {
                if (current_char == key_value_tuples_delimiter) {
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

                if (key_appeared) {
                    key.push_back(current_char);
                } else {
                    value.push_back(current_char);
                }
            }
        }
    }


    void http_request_parser_post::FormDataBody::parse_request_body(http_request &post_request, const vector<string> &raw_request_body, const ContentType &obj) {
        ///TODO: Дописать парсер FormData body
        string boundary = get_request_body_boundary(obj);

        for (const auto &current_line : raw_request_body) {
            if (current_line == boundary) {

            }
        }
    }

    string http_request_parser_post::FormDataBody::get_request_body_boundary(const ContentType &obj) {
        for (auto current_parameter : obj.parameters) {
            if (current_parameter.first == "boundary") {
                return current_parameter.second;
            }
        }

    }

    vector<string> http_request_parser_post::get_request_body(const vector<string> &raw_request) {
        vector<string> raw_request_body;

        unsigned long index_start_of_body;

        for (unsigned long current_request_line_number = 0; current_request_line_number < raw_request.size(); current_request_line_number++) {
            if (raw_request[current_request_line_number] == "\r\n") {
                index_start_of_body = current_request_line_number + 1;
            }
        }

        copy(raw_request.begin() + index_start_of_body, raw_request.end(), back_inserter(raw_request_body));

        return raw_request_body;
    }

    void http_request_parser_post::parse_post_request(http_request& post_request, const vector<string> &raw_request) {
        Request_body* post_request_body;

        const vector<string>& raw_request_body = get_request_body(raw_request);

        ContentType obj = parse_content_type_header(post_request.get_header("Content-Type"));

        detect_request_body_content_type(post_request_body, obj);

        post_request_body->parse_request_body(post_request, raw_request_body, obj);

        delete post_request_body;
    }
}