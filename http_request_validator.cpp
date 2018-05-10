#include <set>
#include "http_request_validator.h"

namespace webserver {
    http_request_validator::http_request_validator() {
        available_request_methods.insert("GET");
        available_request_methods.insert("POST");
    }

    bool http_request_validator::check_request_line_method(const string &request_method) {
        //TODO: проверить что делать если метод неправильный (присылать 501 или нет в ответ)
        if (available_request_methods.find(request_method) != available_request_methods.end()) {
            return true;
        }
        return false;
    }

    bool http_request_validator::check_request_line_url(const string& request_url, const http_request& request) {
        if (request_url[0] == '/') {
            //TODO: выяснить, где проверять этот пункт
            //   1. If Request-URI is an absoluteURI, the host is part of the
            //     Request-URI. Any Host header field value in the request MUST be
            //     ignored.
            //
            //   2. If the Request-URI is not an absoluteURI, and the request includes
            //     a Host header field, the host is determined by the Host header
            //     field value.
            //
            //   3. If the host as determined by rule 1 or 2 is not a valid host on
            //     the server, the response MUST be a 400 (Bad Request) error message.
            const vector<http_header>& request_headers = request.get_headers();
            for (auto current_header : request_headers) {
                if (current_header.type == "Host") {
                    return true;
                }
            }
        }

        if (request_url.substr(0, 7) == "http://") {
            return true;
        }

        return false;
    }

    bool http_request_validator::check_request_line_http_version(const string &request_http_version) {
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

    bool http_request_validator::check_request_line(const http_request& request) {
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

    bool http_request_validator::check_request(const http_request& request) {
        //TODO: сделать проверку request line на лишние символы
        if (!check_request_line(request)) {
            return false;
        }

        return true;
    }
}
