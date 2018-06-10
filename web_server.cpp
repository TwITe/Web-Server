#include "web_server.h"

namespace webserver {
    web_server::web_server(unsigned short int port, const vector<web_handler>& handlers) : handlers(handlers), server(port, convert_client_message, allowed_connections_number) {}

    http_response web_server::generate_400_error_response(const http_request& request) {
        _Pragma("GCC diagnostic push")
        _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
        function<webserver::http_response(webserver::http_request)> error_400_handler = [&](webserver::http_request request) {
            webserver::http_response response;

            string error_message = "Bad Request";
            response.set_response_body(error_message);
            response.set_response_http_code(400);
            response.set_response_length(error_message.size());

            return response;
        };
        _Pragma("GCC diagnostic pop")

        web_handler error_handler("", "", error_400_handler);

        const function<http_response(http_request)>& handler = error_handler.get_transform_to_response_function();

        http_response response = handler(request);
        return response;
    }

    //Запуск web-server.
    //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
    void web_server::start() {
        server.start();
    }

    void web_server::stop() {
        server.stop();
    }
}