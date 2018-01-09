#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "request_param.h"
#include "http_request.h"
#include "http_header.h"
#include "http_request_parser.h"
#include "http_response.h"
#include "tcp_server.h"
#include "web_handler.h"

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

        vector<web_handler> handlers; // обработчики
    public:
        web_server(unsigned short int port, vector<web_handler> handlers);
    private:
        function<string(char*)> convert_client_message = [&](char* request_char_buffer) -> char* {
            string converted_client_message(request_char_buffer);

            vector<string> message_fields;

            string buffer;
            for (auto it = converted_client_message.begin(); it != converted_client_message.end(); it++) {
                if (*it != '\n' || it != converted_client_message.end()) {
                    buffer.push_back(*it);
                }
                else {
                    message_fields.push_back(buffer);
                    buffer.clear();
                }
            }

            http_request_parser parser;
            http_request received_request = parser.parse(message_fields);


            char* return_response = new char[128000];
            return return_response;
        };
    public:
        //Запуск web-server.
        //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
        void start();

        void stop();

        vector<web_handler> get_handlers() {
            return handlers;
        }
    };
}

#endif //WEB_SERVER_H