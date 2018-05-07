#include <set>
#include "request_syntax_validator.h"

namespace webserver {
    request_syntax_validator::request_syntax_validator() {
        available_headers.insert("Accept");
        available_headers.insert("Accept-Charset");
        available_headers.insert("Accept-Encoding");
        available_headers.insert("Accept-Language");
        available_headers.insert("Authorization");
        available_headers.insert("Expect");
        available_headers.insert("From");
        available_headers.insert("Host");
        available_headers.insert("If-Match");
        available_headers.insert("If-None-Match");
        available_headers.insert("If-Modified-Since");
        available_headers.insert("If-Range");
        available_headers.insert("If-Unmodified-Since");
        available_headers.insert("Max-Forwards");
        available_headers.insert("Proxy-Authorization");
        available_headers.insert("Range");
        available_headers.insert("Referer");
        available_headers.insert("TE");
        available_headers.insert("User-Agent");

        available_methods.insert("GET");
        available_methods.insert("HEAD");
        available_methods.insert("POST");
        available_methods.insert("PUT");
        available_methods.insert("DELETE");
        available_methods.insert("CONNECT");
        available_methods.insert("OPTIONS");
        available_methods.insert("TRACE");
    }

    bool request_syntax_validator::check_request_method(const string& request_method) {
        if (available_methods.find(request_method) != available_methods.end()) {
            return true;
        }
        return false;
    }

    bool request_syntax_validator::check_request_http_version(const string& request_http_version) {
        int normal_http_version_min_length = 8;
        if (request_http_version.length() < normal_http_version_min_length) {
            return false;
        }
        if (request_http_version.substr(0, 5) != "HTTP/") {
            return false;
        }
        int major_number = -1;
        int minor_number = -1;

        //парсинг номера версии http
        string buffer;
        for (int i = 5; i < request_http_version.length(); i++) {
            char current_char = request_http_version[i];
            if ('0' <= current_char && current_char <= '9') {
                buffer.push_back(current_char);
            }
            else if (current_char == '.') {
                major_number = stoi(buffer);
                buffer.clear();
            }
            else if (i == request_http_version.length() - 1) {
                minor_number = stoi(buffer);
            }
            else {
                return false;
            }
        }

        return major_number != -1 && minor_number != -1;
    }

    bool request_syntax_validator::check_request_headers(const http_request& request) {
        const vector<http_header>& headers = request.get_headers();

        for (const auto &current_header : headers) {
            if (available_headers.find(current_header) == available_headers.end()) {
                return false;
            }
        }

        return true;
    }

    bool request_syntax_validator::check_request_line(const http_request& request) {
        bool method_is_valid = false;
        const string &current_request_method = request.get_request_method();
        if (!check_request_method(current_request_method)) {
            return false;
        }

        const string &current_request_http_version = request.get_request_http_version();
        if (!check_request_http_version(current_request_http_version)) {
            return false;
        }

        return true;
    }
}