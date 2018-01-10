#include "http_router.h"

namespace webserver {
    string http_router::get_request_pattern(const http_request& request) {
        string pattern;
        const string& url = request.get_request_url();

        size_t request_params_begin_position = url.find('?');
        size_t http_prefix_end_position = url.find("://");

        string shrinked_url;

        if (request_params_begin_position != string::npos) {
            shrinked_url = url.substr(http_prefix_end_position, request_params_begin_position);
        }
        else {
            shrinked_url = url.substr(http_prefix_end_position);
        }

        size_t host_end_position = shrinked_url.find('/');

        shrinked_url = shrinked_url.substr(host_end_position);

        pattern = shrinked_url;

        return pattern;
    }

    vector<web_handler>::iterator http_router::get_suited_request_handler(vector<web_handler>& handlers, const http_request& request) {
        const string& client_request_method = request.get_request_method();
        string client_request_pattern = get_request_pattern(request);

        vector<web_handler>::iterator suitable_web_handler_iterator = handlers.end();

        for (auto current_web_handler = handlers.begin(); current_web_handler != handlers.end(); current_web_handler++) {
            string current_web_handler_pattern = current_web_handler->get_web_handler_pattern();
            string current_web_handler_method = current_web_handler->get_web_handler_method();
            if (current_web_handler_method == client_request_method && current_web_handler_pattern == client_request_pattern) {
                suitable_web_handler_iterator = current_web_handler;
            }
        }
        return suitable_web_handler_iterator;
    }
}