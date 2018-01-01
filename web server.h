#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <iostream>
#include <string>
#include <vector>
#include <functional>
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
    private:
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

    //Главный класс веб-сервера
    class web_server {
    private:
        //Порт, на котором требуется принимать соединения
        int port;

        //Действия, которые нужно выполнять при приёме соединения
        vector<web_handler> handlers;

    public:
        web_server(int port, vector<web_handler> handlers) :
                port{port}, handlers{handlers} {};

        //Запуск web-server.
        //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
        void start();

        //Остановка web-server
        void stop();
    };
}