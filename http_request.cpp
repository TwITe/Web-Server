#include "http_request.h"
using namespace std;

namespace webserver {
    void http_request::set_http_request_method(const string& client_requst_method) {
        method = client_requst_method;
    }

    void http_request::set_http_request_url(const string& client_requst_url) {
        url = client_requst_url;
    }

    void http_request::set_http_version(const string& client_http_version) {
        http_version = client_http_version;
    }

    void http_request::set_http_request_body(const string& client_request_body) {
        request_body = client_request_body;
    }

    void http_request::add_http_request_param(request_param& client_request_param) {
        request_params.push_back(client_request_param);
    }

    void http_request::add_http_request_header(http_header& client_request_header) {
        headers.push_back(client_request_header);
    }

    const vector<http_header>& http_request::get_headers() const {
        return headers;
    }

    const vector<request_param> http_request::get_request_params() const {
        return request_params;
    }

    const string& http_request::get_request_body() const {
        return request_body;
    }

    const string& http_request::get_request_url() const {
        return url;
    }

    const string& http_request::get_request_method() const {
        return method;
    }

    const string& http_request::get_request_http_version() const {
        return http_version;
    }


}