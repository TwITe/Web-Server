#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include "http_request.h"
#include "http_header.h"
#include "http_request_parser.h"

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
    //Класс, который инкапсулирует в себе http ответ: HTTP заголовки,
    //размер данных, тело данных, content-type
    class http_response {
    private:
        //заголовки HTTP ответа
        vector<http_header> headers;

        //Код ответа: 200, 400...
        int http_code;

        //Тело ответа
        string response_body;

        //Размер response_body
        unsigned long content_length;
    public:
        void set_response_http_code(int code) {
            http_code = code;
        }

        void set_response_body(const string& response) {
            response_body = response;
        }

        void set_response_length(unsigned long response_length) {
            content_length = response_length;
        }

        void set_response_headers(const vector<http_header>& http_headers) {
            headers = http_headers;
        }
    };

    class tcp_server {
    private:
        unsigned short int PORT;

        int listener_socket;

        struct sockaddr_in server_address{};

        socklen_t socket_length;

        function<vector <string>(char*)> convert_client_message;

    public:
        tcp_server(unsigned short int PORT, function<vector<string>(char*)> convert_client_message) : PORT(PORT),
        convert_client_message(convert_client_message) {};

        bool start() {
            memset(&server_address, 0, sizeof(server_address));

            listener_socket = socket(AF_INET, SOCK_STREAM, 0);

            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(PORT);
            if (inet_aton("127.0.0.1", &server_address.sin_addr) == 0) {
                cout << "[Server] Invalid IP address" << endl;
                return false;
            }

            if (bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
                cout << "[Server] Binding failed" << endl;
                return false;
            }

            if (listen(listener_socket, 30) == -1) {
                cout << "[Server] Listening failed" << endl;
                return false;
            }

            cout << "[Server] All setting are done" << endl;
            cout << "[Server] Server enabled" << endl;

            take_requests();
        }

    private:
        void handle_client_http_request(vector<string> client_message) {
            http_request_parser parser;

            http_request request = parser.parse(client_message);
        }

        void handle_client(int socket) {
            char read_buffer[128000];

            while (true) {
                recv(socket, read_buffer, sizeof(read_buffer), 0);
                cout << "[Server] Client message accepted" << endl;
                cout << "[Server] Client message: " << read_buffer << endl;

                handle_client_http_request(convert_client_message(read_buffer));

                memset(&read_buffer, 0, sizeof(read_buffer));
            }
        };

        void connection_handler(int client_socket) {
            cout << "----------------------------" << endl << endl;
            cout << "[Server] Connection accepted" << endl << endl;
            cout << "----------------------------" << endl << endl;

            handle_client(client_socket);
        }

        void take_requests() {
            for (int i = 0; i > 0; i++) {
                int client_socket;
                client_socket = accept(listener_socket, (struct sockaddr*)& server_address, &socket_length);

                cout << "----------------------------" << endl << endl;
                cout << "[Server] Connection accepted" << endl << endl;
                cout << "----------------------------" << endl << endl;

                thread handling_thread(&tcp_server::connection_handler, this, client_socket);
                handling_thread.detach();
            }
        }
    };

    //Базовый обработчик всех запросов
    class web_handler {
    private:
        //шаблон, по которому определяем, нужно ли применить данный обработчик к данному запросу.
        //например, /items - в этом случае если прийдёт запрос http://localhost:80/items?par=32 , нужно применить данный обработчик
        string pattern;

        //Для какого метода задаётся данный обработчик - GET, POST, и т.д.
        string method;

        //Функция, которая преобразует http_request в http_response.
        //Функция задаётся пользователем, но web-server обязан корректно инициализировать http_reqeust.
        function<http_response(http_request)> handler;
    public:
        web_handler(string pattern, string method, function<http_response(http_request)> handler) :
                pattern{pattern}, method{method}, handler{handler} {}
    };

    class web_server {
    private:
        const unsigned short int PORT;

        vector<web_handler> handlers; // обработчики
    public:
        web_server(unsigned short int port, vector<web_handler> handlers) :
                PORT(port), handlers(handlers) {};
    private:
        function<vector <string>(char*)> convert_client_message = [&](char* request_char_buffer) {
            string converted_client_message(request_char_buffer);

            vector <string> message_fields;

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
            return message_fields;
        };

    public:
        //Запуск web-server.
        //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
        void start() {
            tcp_server server(PORT, convert_client_message);
            if (!server.start()) {
                throw runtime_error("Failed to start TCP Server");
            }
        };

        void stop() {
            return;
        }
    };
}

#endif //WEB_SERVER_H