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
        function<vector <string>(char*)> convert_client_message;
    public:
        //Запуск web-server.
        //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
        void start();

        void stop();
    };
}

#endif //WEB_SERVER_H