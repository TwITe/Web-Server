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
#include "http_request_validator.h"
#include "http_response_builder.h"
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
        vector<web_handler> handlers;

        http_router request_handler_router;

        tcp_server server;

        http_request_parser parser;

//        http_request_validator request_syntax_validator;

        http_response_builder response_builder;

        http_response generate_400_error_response(const http_request& request);

        function<string(char*)> convert_client_message = [&](char* request_char_buffer) -> string {
            string raw_client_message(request_char_buffer);

            vector<string> message_fields;
            string buffer;

            for (auto it = raw_client_message.begin(); it != raw_client_message.end(); it++) {
                if (*it == '\r' && *(it + 1) == '\n') {
                    message_fields.emplace_back("\r\n");
                    it+=1;
                }
                else {
                    buffer.push_back(*it);
                    if ((*(it + 1) == '\r' && *(it + 2) == '\n')) {
                        message_fields.emplace_back(buffer);
                        buffer.clear();
                        it+=2;
                    }

                    if (it + 1 == raw_client_message.end()) {
                        message_fields.emplace_back(buffer);
                        buffer.clear();
                    }
                }
            }

            http_request request = parser.parse_request(message_fields);

            http_response response;

//            if (!request_syntax_validator.check_request(request)) { // должна быть другая работа валидатора: он будет возвращать код ошибки, и в веб сервере
            //генерироваться требуемый response по коду ошибки
//                response = generate_400_error_response(request);
//            }
//            else {
                web_handler suitable_web_handler = request_handler_router.get_suitable_request_handler(handlers, request);
                const function<http_response(http_request)>& handler = suitable_web_handler.get_transform_to_response_function();

                response = handler(request);
//            }

            string server_response = response_builder.build_response(response);
            return server_response;
        };
    public:
        web_server(unsigned short int port, const vector<web_handler>& handlers);

        //Запуск web-server.
        //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
        void start();

        void stop();
    };
}

#endif //WEB_SERVER_H