#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
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
#include <sstream>

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

    //Тип данных - HTTP заголовок
    struct http_header {
        //Тип заголовка (например, Content-type)
        string type;

        //Значение заголовка (например, text/html; charset=utf-8)
        string value;
    };

    //Один из параметров, переданных в URL
    struct request_param {
        //Имя параметра
        string name;

        //Значение параметра
        string value;
    };

    //Класс, который инкапсулирует в себе http запрос: HTTP заголовки,
    //URL запроса, request-параметры, тело запроса (в случае POST запроса), тип запроса - GET, POST, PUT
    class http_request {
    protected:
        //заголовки HTTP запроса
        vector<http_header> headers;

        //Параметры запроса (например, если запрос был http://localhost:8083/comand?param=1&param2=2 ,
        //То нужно в этот вектор положить 2 пары: param и 1; param2 и 2
        vector<request_param> request_params;

        //тело запроса в кодировке, определенной в заголовке Content-Type. По умолчанию - utf-8
        //Тело может отсутствовать, например, если был GET запрос
        string request_body;

        //URL запроса. Например, http://localhost:8083/comand?param=1&param2=2
        string url;

        //Метод запроса - POST, GET, PUT, PATCH, HEADER...
        string method;
    public:

    };

    class http_request_parser {
    public:
        static http_request parse(const string& request_message) {
            http_request request;
            stringstream ss;

            return request;
        }
    };

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

    class connection_handler {
    private:
        int socket;
        http_request request;
        http_response response;
    public:
        connection_handler(int new_socket) : socket(new_socket) {
            cout << "----------------------------" << endl << endl;
            cout << "[Server] Connection accepted" << endl << endl;
            cout << "----------------------------" << endl << endl;

            handle_client();
        }

        void handle_message(string client_message) {

        }

        void handle_client() {
            ssize_t message_size;
            char read_buffer[256000];
            while ((message_size = recv(socket, read_buffer, sizeof(read_buffer), 0)) > 0) {
                cout << "[Server] Client message accepted" << endl;
                cout << "[Server] Client message: " << read_buffer << endl;
                cout.flush();

                handle_message(convert_client_message_char_buffer_to_string(read_buffer));

                memset(&read_buffer, 0, sizeof(read_buffer));
            }
        };

        string convert_client_message_char_buffer_to_string(char request_char_buffer[]) {
            string request(request_char_buffer);

            return request;
        }

        ~connection_handler() = default;
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
        web_handler(string pattern, string method, function<http_response(http_request)>) :
                pattern{pattern}, method{method}, handler{handler} {}
    };

    class tcp_server {
    private:
        unsigned short int PORT;

        int listener_socket;

        struct sockaddr_in server_address{};

        socklen_t client_len;
    public:
        tcp_server(unsigned short int PORT) : PORT(PORT) {};

        void activate_tcp_server() {
            memset(&server_address, 0, sizeof(server_address));

            listener_socket = socket(AF_INET, SOCK_STREAM, 0);

            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(PORT);
            if (inet_aton("127.0.0.1", &server_address.sin_addr) == 0) {
                cout << "[Server] Invalid IP address" << endl;
                return;
            }

            if (bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
                cout << "[Server] Binding failed" << endl;
                return;
            }

            if (listen(listener_socket, 100) == -1) {
                cout << "[Server] Listening failed" << endl;
                return;
            }

            cout << "[Server] All setting are done" << endl;
            cout << "[Server] Server enabled" << endl;

            take_requests();
        }


        void take_requests() {
            for (int i = 0; i > 0; i++) {
                int client_socket = accept(listener_socket, (struct sockaddr*) &server_address, &client_len);
                cout << "----------------------------" << endl << endl;
                cout << "[Server] Connection accepted" << endl << endl;
                cout << "----------------------------" << endl << endl;

                thread handling_thread(connection_handler::connection_handler(client_socket));
                handling_thread.detach();
            }
        }
    };

    //Главный класс веб-сервера
    class web_server {
    private:
        //Порт, на котором требуется принимать соединения
        const unsigned short int PORT;

        //Действия, которые нужно выполнять при приёме соединения
        vector<web_handler> handlers;

    public:
        web_server(unsigned short int port, vector<web_handler> handlers) :
                PORT(port), handlers(handlers) {};

        //Запуск web-server.
        //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
        void start() {
            tcp_server server(PORT);
            server.activate_tcp_server();


        };

        void stop() {
            return;
        }
    };


}