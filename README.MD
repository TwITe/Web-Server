# Веб Сервер

## Вступление
Имплементация простого веб сервера

## Установка

Клонируйте данный репозиторий с **GitHub**:

```
mkdir ~/web_server
cd ~/web_server
git clone https://github.com/TwITe/Web-Server.git
```

## Запуск веб-сервера:

1. Перед запуском сервера создайте вектор, который будет хранить веб хандлеры, обрабатывающие запросы:
```c++
vector<webserver::web_handler> handlers;
```
2. Создайте необходимые обработчики

**Создание обработчика:**
 - Создайте функцию, которая будет преобразовывать request в response
```c++
    function<webserver::http_response(webserver::http_request)> index_handler = [&](webserver::http_request request) {
        webserver::http_response response;

        string response_body = "Hello, World!";

        response.set_response_body(response_body);
        response.set_response_http_code(200);
        response.set_response_length(response_body.size());

        vector<webserver::http_header> headers;
        headers.push_back(webserver::http_header{"Content-Type", "text/plain"});
        response.set_response_headers(headers);

        return response;
    };
```
 - Создайте веб хандлер
```c++
 webserver::web_handler index_web_handler("/index", "GET", index_handler);
```

3. Поместите созданные обработчики в вектор
```c++
    handlers.emplace_back(index_web_handler);
```

4. Создайте веб сервер, указав порт и вектор с обработчиками
```c++
webserver::web_server server(PORT, handlers);
```

5. Запустите веб сервер
```c++
server.start();
```

## API Веб сервера

Функции для девелоперов, использующих данный веб сервер:

### Request
Сервер предоставляет следующее API для работы с объектами типа **http_request**:

