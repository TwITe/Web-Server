#ifndef WEB_WEB_HANDLER_H
#define WEB_WEB_HANDLER_H

#include "http_response.h"
#include "http_request.h"
#include <functional>
using namespace std;

namespace webserver {
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
        web_handler(string pattern, string method, function<http_response(http_request)> handler);

        const string& get_web_handler_pattern() const;

        const string& get_web_handler_method() const;

        http_response transform_request_to_response(const http_request& received_request);

        bool empty();
    };
}

#endif //WEB_WEB_HANDLER_H
