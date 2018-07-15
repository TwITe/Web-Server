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

1. Перед запуском сервера создайте вектор, который будет хранить обработчики типа **web_handler**, обрабатывающие запросы:
```c++
vector<webserver::web_handler> handlers;
```
2. Создайте необходимые обработчики

**Создание обработчика:**
 * Создайте функцию, которая будет преобразовывать request в response
```c++
    function<webserver::http_response(webserver::http_request)> index_handler =
    [&](webserver::http_request request) {
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
 * Создайте обработчик, инициализировав его следующим образом:
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

Функции для написания обработчиков для девелоперов, использующих данный веб сервер:

### Request
Сервер предоставляет следующее API для работы с объектами типа **http_request**:

1) **const map<string, string>& get_request_body() const**

Возвращает тело запроса в виде **const map<string, string>&**

Пример использования:
```c++
    webserver::http_request request;
    const map<string, string>& requestBody = request.get_request_body();

    for (auto currentField : requestBody) {
        const string& key = currentField.first;
        const string& value = currentField.second;
        ...
    }

    return 0;
```

2) **const vector<request_param> get_request_params() const**

Возвращает query параметры в виде **const vector<request_param>&**

Пример использования:
```c++
    webserver::http_request request;
    const vector<webserver::request_param> requestParams = request.get_request_params();

    for (auto currentParam : requestParams) {
        const string& paramName = currentParam.name;
        const string& paramValue = currentParam.value;
        ...
    }

    return 0;
```

3) **const vector<http_header>& get_headers() const**

Возвращаеты все хэдеры запроса в виде **const vector<http_header>&**

Пример использования:
```c++
    webserver::http_request request;
    const vector<webserver::http_header> requestHeaders = request.get_headers();

    for (auto currentHeader : requestHeaders) {
        const string& headerType = currentHeader.type;
        const string& headerValue = currentHeader.value;
        ...
    }

    return 0;
```

4) **const http_header get_header(const string& header_name) const**

Принимает на вход тип запрашиваемого хэдера. В случае существования хэдера такого типа возвращает данный хэдер, иначе - возвращает пустой хэдер

Пример использования:
```c++
    webserver::http_request request;
    const webserver::http_header& requestHeader = request.get_header("Host");

    const string& headerType = requestHeader.type;
    const string& headerValue = requestHeader.value;
    ...

    return 0;
```

5) **const string& get_request_url()**, **const string& get_request_method()**, **const string& get_request_http_version()**

Данные методы возвращают *полный URL*, *Метод запроса*, *HTTP версию запроса* соответственно

### Response
Сервер предоставляет следующее API для работы с объектами типа **http_response**:

1) **void set_response_http_code(int code)**

Метод принимает на вход HTTP код ответа типа *int* и устанавливает его

Пример использования:
```c++
    webserver::http_response response;
    response.set_response_code(200);
    ...

    return 0;
```

2) **void set_response_body(const string& response)**

Метод принимает *string* представление тела ответа и устанавливает его

Пример использования:
```c++
    webserver::http_response response;

    const string responseBody = "Hello, World!";

    response.set_response_body(responseBody);
    ...

    return 0;
```

3) **void set_response_length(unsigned long response_length)**

Метод принимает на вход длину ответа типа *unsigned long* и устанавливает его

Пример использования:
```c++
    webserver::http_response response;

    const string responseBody = "Hello, World!";

    response.set_response_length(responseBody.length());
    ...

    return 0;
```

4) **void set_response_headers(const vector<http_header>& http_headers)**

Метод принимает на вход хэдеры вектор хэдеров типа *vector<http_header>* и устанавливает их

Пример использования:
```c++
    webserver::http_response response;

    webserver::http_header responseHeader;
    responseHeader.type = "Content-Type";
    responseHeader.value = "application/json";

    vector<webserver::http_header> responseHeaders;
    responseHeaders.emplace_back(responseHeader);

    response.set_response_headers(responseHeaders);
    ...

    return 0;
```

## Бенчмарки
Сравнение данного веб сервера и стандартного HTTP 1.1 веб сервера языка Go.

Сравнение производилось специальной программой, предназначенной для нагрузочного тестирования: https://github.com/blinky-z/ServerLoadTesting.

C++ Сервер, созданный для тестирования на основе данного данного сервера: https://github.com/blinky-z/LoadSite

Сырые данные, которые были обработаны и на основе их была произведена аналитика: https://github.com/blinky-z/ServerLoadTesting/tree/master/BenchmarkRawData.
Данные были обработаны в Microsoft Excel 2016

Cравнение серверов будет производиться по таким показателям, как
* **Среднее время ответа**: Чем ниже - тем лучше
* **Медиана времени ответа**: Чем ниже - тем лучше
* **95 перцентиль времени ответа**: Чем ниже - тем лучше

### Тестирование с большим количеством клиентов (имитация реальной работы сайта)

**Условия тестирования:**
1) *Начальное количество клиентов*: 10
2) *Максимальное количество клиентов*: 300
3) *Постепенно добавляется по 10 клиентов до достижения лимита клиентов*
4) *Задержка между запросами*: 0.7 ms

#### Общее сравнение двух серверов:
В данном опыте сравниваются все показатели обоих серверов за все время работы сервера

![Общее сравнение двух серверов](https://i.imgur.com/G05sJXA.png)

#### Статистика роста среднего времени ответа в зависимости от количества клиентов:
В данном опыте сравнивается изменение среднего времени ответа сервера в соответствии с ростом клиентов.     Чем медленнее растет показатель - тем лучше

![Статистика роста среднего времени ответа в зависимости от количества клиентов](https://i.imgur.com/i5a3w3h.png)

#### Статистика роста медианы времени ответа в зависимости от количества клиентов:
В данном опыте сравнивается изменение медианты времени ответа сервера в соответствии с ростом клиентов. Чем медленнее растет показатель - тем лучше

![Статистика роста медианы времени ответа в зависимости от количества клиентов](https://i.imgur.com/kOWlwV9.png)

#### Статистика роста 95 перцентиля времени ответа в зависимости от количества клиентов.
В данном опыте сравнивается изменение 95 перцентиля времени ответа сервера в соответствии с ростом клиентов. Чем медленнее растет показатель - тем лучше

![Статистика роста 95 перцентиля времени ответа в зависимости от количества клиентов](https://i.imgur.com/4chbL1y.png)

### Тестирование с большим количеством запросов с каждого клиента (имитация веб сервиса)

**Условия тестирования:**
1) *Начальное количество клиентов*: 8
2) *Количество клиентов фиксировано и не меняется*
4) *Задержка между запросами*: 0.3 ms

#### Общее сравнение двух серверов:
В данном опыте сравниваются все показатели обоих серверов за все время работы сервера

![Общее сравнение двух серверов](https://i.imgur.com/aJsgZr3.png)

