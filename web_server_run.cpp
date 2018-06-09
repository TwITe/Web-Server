#include "web_server.h"
using namespace std;

int main() {
    vector<webserver::web_handler> handlers;

    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
    function<webserver::http_response(webserver::http_request)> index_handler = [&](webserver::http_request request) {
        webserver::http_response response;

        string response_body = "Hello, World!";

        response.set_response_body(response_body);
        response.set_response_http_code(200);
        response.set_response_length(response_body.size());

        vector<webserver::http_header> headers;
        headers.push_back(webserver::http_header{"Content-Type", "text/plain"});
        response.set_response_headers(headers);

        return response;
    };
    _Pragma("GCC diagnostic pop")

    webserver::web_handler index_web_handler("/index", "GET", index_handler);

    handlers.emplace_back(index_web_handler);

    webserver::web_server server(8080, handlers);

    server.start();
}