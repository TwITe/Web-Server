#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "request_param.h"
#include "http_request.h"
#include "http_header.h"
#include "http_request_parser.h"
#include "http_response.h"
#include "tcp_server.h"
#include "web_handler.h"
#include "http_router.h"
#include <map>

using namespace std;
//Реализация простого веб-сервера.
//
//Каждое соединенеи должно обрабатываться в отдельном thread (потоке).
//Соединения на входе принимает отдельный, выделенный поток (thread), и передаёт исполнение созданному специально для этого запроса потоку.
//
//Веб-сервер поддерживает описание Роутов (routes) с помощью лямбда-функций.
//Веб-сервер сам инициализирует http_request объект и передаёт исполнение в лямбду.
//
//Веб-сервер должен выдерживать 30 одновременных соединений.

namespace webserver {
    class web_server {
    private:
        const unsigned short int PORT;

        vector<web_handler> handlers;

        http_router request_handler_router;

        tcp_server server;

        map<int, string> reason_phrases;
    public:
        web_server(unsigned short int port, const vector<web_handler>& handlers);
    private:
        function<string(char*)> convert_client_message = [&](char* request_char_buffer) -> string {
            string converted_client_message(request_char_buffer);

            vector<string> message_fields;

            string buffer;
            for (auto it = converted_client_message.begin(); it != converted_client_message.end(); it++) {
                if (*it != '\n' || it != converted_client_message.end()) {
                    buffer.push_back(*it);
                }
                else {
                    message_fields.emplace_back(buffer);
                    buffer.clear();
                }
            }

            http_request_parser parser;
            http_request request = parser.parse(message_fields);

            web_handler suitable_web_handler = request_handler_router.get_suitable_request_handler(handlers, request);

            http_response response = suitable_web_handler.transform_request_to_response(request);

            string response_http_version = "HTTP/1.1";
            int response_status_code = response.get_response_code();
            string response_reason_phrase = reason_phrases[response_status_code];
            string response_status_line =
                    response_http_version + " " + to_string(response_status_code) + " " + response_reason_phrase + "\n";

            string converted_response;
            converted_response += response_status_line;
            vector<http_header> response_headers = response.get_response_headers();

            for (const http_header& current_header : response_headers) {
                converted_response += current_header.type + ": " + current_header.value + "\n";
            }

            bool is_response_message_body_exists;

            unsigned long response_body_length = response.get_content_length();

            is_response_message_body_exists = response_body_length != 0;

            if (is_response_message_body_exists) {
                http_header entity_content_length_header;

                entity_content_length_header.type = "Content-Length";
                entity_content_length_header.value = to_string(response_body_length);

                const string& response_message_body = response.get_response_body();

                converted_response += entity_content_length_header.type + ": " + entity_content_length_header.value + "\n";

                converted_response += '\n';

                converted_response += response_message_body;
            }

            return converted_response;
        };
    public:
        //Запуск web-server.
        //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
        void start();

        void stop();
    };
}

#endif //WEB_SERVER_H