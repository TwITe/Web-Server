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
        void set_http_request_method(const string& client_requst_method) {
            method = client_requst_method;
        }

        void set_http_request_url(const string& client_requst_url) {
            url = client_requst_url;
        }

        void set_http_request_body(const string& client_request_body) {
            request_body = client_request_body;
        }

        void add_http_request_param(request_param& client_request_param) {
            request_params.push_back(client_request_param);
        }

        void add_http_request_header(http_header& client_request_header) {
            headers.push_back(client_request_header);
        }

        string get_url_second_part_for_extending_url_function() {
            return url;
        }

        const vector<http_header>& getHeaders() const {
            return headers;
        }

        const vector<request_param> get_request_params() const {
            return request_params;
        }

        const string& get_request_body() const {
            return request_body;
        }

        const string& get_request_url() const {
            return url;
        }

        const string& get_request_method() const {
            return method;
        }
    };

    class http_request_parser {
        vector<string> socket_message;
    public:
        http_request_parser(vector<string>& received_socket_message) : socket_message(received_socket_message) {};
    private:
        void parse_request_line(http_request& request) {
            istringstream request_line(socket_message[0]);
            string method;
            string request_url;

            request_line >> method >> request_url;

            request.set_http_request_method(method);
            request.set_http_request_url(request_url);
        }

        void extend_request_url_by_host(const string& host, http_request& request) {
            string full_url;
            string url_second_part = request.get_url_second_part_for_extending_url_function();
            full_url = host + url_second_part;
            request.set_http_request_url(full_url);
        }

        bool check_is_current_header_host(const string& current_header) {
            return current_header == "Host";
        }

        bool check_is_current_header_request_body(const string& current_header) {
            return current_header == "Content-type";
        }

        bool parse_headers(http_request& request) {
             for (auto current_message_line = socket_message.begin() + 1; current_message_line != socket_message.end(); current_message_line++) {
                 string current_header_type;
                 string current_header_value;
                 for (size_t current_char_postion = 0; current_char_postion < current_message_line->size(); current_char_postion++) {
                     if ((*current_message_line)[current_char_postion] == ' ') {
                         current_header_value = current_message_line->substr(current_char_postion + 1);
                         break;
                     }
                     current_header_type.push_back((*current_message_line)[current_char_postion]);
                 }
                 if (check_is_current_header_host(current_header_type)) {
                     extend_request_url_by_host(current_header_value, request);
                 }
                 if (check_is_current_header_request_body(current_header_type)) {
                     request.set_http_request_body(current_header_value);
                 }
                 else {
                     http_header current_header;
                     current_header.type = current_header_type;
                     current_header.value = current_header_value;
                     request.add_http_request_header(current_header);
                 }
            }
        }
    public:
        //TODO: Написать тесты для парсера
        http_request parse() {
            http_request request;

            parse_request_line(request);
            parse_headers(request);

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

        function<vector <string>(char*)> convert_client_message;
    public:
        connection_handler(int new_socket, function<vector <string>(char*)> convert_client_message) : socket(new_socket),
        convert_client_message(convert_client_message) {
            cout << "----------------------------" << endl << endl;
            cout << "[Server] Connection accepted" << endl << endl;
            cout << "----------------------------" << endl << endl;

            handle_client();
        }

        ~connection_handler() = default;

    private:
        void handle_message(vector <string> client_message) {
            http_request_parser parser(client_message);
            http_request request = parser.parse();
        }

        void handle_client() {
            char read_buffer[256000];
            while ((recv(socket, read_buffer, sizeof(read_buffer), 0)) > 0) {
                cout << "[Server] Client message accepted" << endl;
                cout << "[Server] Client message: " << read_buffer << endl;
                cout.flush();

                handle_message(convert_client_message(read_buffer));

                memset(&read_buffer, 0, sizeof(read_buffer));
            }
        };
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

    class tcp_server {
    private:
        unsigned short int PORT;

        int listener_socket;

        struct sockaddr_in server_address{};

        socklen_t client_len;

        function<vector <string>(char*)> convert_client_message;

    public:
        tcp_server(unsigned short int PORT, function<vector <string>(char*)> convert_client_message) : PORT(PORT),
        convert_client_message(convert_client_message) {};

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
    private:
        void take_requests() {
            for (int i = 0; i > 0; i++) {
                int client_socket = accept(listener_socket, (struct sockaddr*) &server_address, &client_len);
                cout << "----------------------------" << endl << endl;
                cout << "[Server] Connection accepted" << endl << endl;
                cout << "----------------------------" << endl << endl;

                connection_handler current_connection_handler(client_socket, convert_client_message);
                thread handling_thread(current_connection_handler);
                handling_thread.detach();
            }
        }
    };

    class web_server {
    private:
        const unsigned short int PORT;

        vector<web_handler> handlers;

    public:
        web_server(unsigned short int port, vector<web_handler> handlers) :
                PORT(port), handlers(handlers) {};

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

        //Запуск web-server.
        //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
        void start() {
            tcp_server server(PORT, convert_client_message);
            server.activate_tcp_server();
        };

        void stop() {
            return;
        }
    };


}