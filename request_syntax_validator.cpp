#include <set>
#include "request_syntax_validator.h"

namespace webserver {
    request_syntax_validator::request_syntax_validator() {
        available_request_methods.insert("GET");
        available_request_methods.insert("POST");
    }

    bool request_syntax_validator::check_request_line_method(const string &request_method) {
        if (available_request_methods.find(request_method) != available_request_methods.end()) {
            return true;
        }
        return false;
    }

    bool request_syntax_validator::check_request_line_url(const string& request_url, const http_request& request) {
        if (request_url[0] == '/') {
            const vector<http_header>& request_headers = request.get_headers();
            for (auto current_header : request_headers) {
                if (current_header.type == "Host") {
                    return true;
                }
            }

            return false;
        }

        if (request_url.substr(0, 7) == "http://") {
            return true;
        }
        return false;
    }

    bool request_syntax_validator::check_request_line_http_version(const string &request_http_version) {
        unsigned long default_http_version_min_length = 8;
        if (request_http_version.length() < default_http_version_min_length) {
            return false;
        }

        if (request_http_version.substr(0, 5) != "HTTP/") {
            return false;
        }

        int major_number = -1;
        int minor_number = -1;

        //парсинг номера версии http
        string buffer;
        for (unsigned long i = 5; i < request_http_version.length(); i++) {
            char current_char = request_http_version[i];
            if ('0' <= current_char && current_char <= '9') {
                buffer.push_back(current_char);
            }
            else if (current_char == '.') {
                major_number = stoi(buffer);
                buffer.clear();
            }
            else {
                return false;
            }
        }
        minor_number = stoi(buffer);

        return major_number != -1 && minor_number != -1;
    }

    bool request_syntax_validator::check_request_line(const http_request& request) {
        const string& request_method = request.get_request_method();
        if (!check_request_line_method(request_method)) {
            return false;
        }

        const string& request_url = request.get_request_url();
        if (!check_request_line_url(request_url, request)) {
            return false;
        }

        const string& request_http_version = request.get_request_http_version();
        if (!check_request_line_http_version(request_http_version)) {
            return false;
        }

        return true;
    }

    bool request_syntax_validator::check_request(const http_request& request, const vector<string>& raw_request) {
        if (!check_request_line(request)) {
            return false;
        }

        return true;
    }
}
