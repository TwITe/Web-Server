#include "http_router.h"

namespace webserver {
    string http_router::get_request_pattern(const http_request& request) {
        string pattern;
        const string& url = request.get_request_url();

        size_t request_params_begin_position = url.find('?');
        size_t http_prefix_end_position = url.find("://") + 3;

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

    web_handler http_router::generate_default_error_handler() {
        function<webserver::http_response(webserver::http_request)> error_index_handler = [&](webserver::http_request request) {
            webserver::http_response response;

            string error_message = "Oops! Something went wrong";
            response.set_response_body(error_message);
            response.set_response_http_code(404);
            response.set_response_length(error_message.size());

            return response;
        };

        web_handler error_handler("", "", error_index_handler);

        return error_handler;
    }

    web_handler http_router::get_suitable_request_handler(vector<web_handler>& handlers, const http_request& request) {
        const string& client_request_method = request.get_request_method();
        string client_request_pattern = get_request_pattern(request);

        web_handler suitable_web_handler("", "", nullptr);

        for (const auto& current_web_handler : handlers) {
            const string& current_web_handler_pattern = current_web_handler.get_web_handler_pattern();
            const string& current_web_handler_method = current_web_handler.get_web_handler_method();
            if (current_web_handler_method == client_request_method && current_web_handler_pattern == client_request_pattern) {
                suitable_web_handler = current_web_handler;
            }
        }

        if (suitable_web_handler.empty()) {
            suitable_web_handler = generate_default_error_handler();
        }

        return suitable_web_handler;
    }
}