#include "web_server.h"

namespace webserver {
    web_server::web_server(unsigned short int port, const vector<web_handler>& handlers) : handlers(handlers), server(port, convert_client_message) {
        reason_phrases[100] = "Continue";
        reason_phrases[101] = "Switching Protocols";
        reason_phrases[200] = "OK";
        reason_phrases[201] = "Created";
        reason_phrases[202] = "Accepted";
        reason_phrases[203] = "Non-Authoritative Information";
        reason_phrases[204] = "No Content";
        reason_phrases[205] = "Reset Content";
        reason_phrases[206] = "Partial Content";
        reason_phrases[300] = "Multiple Choices";
        reason_phrases[301] = "Moved Permanently";
        reason_phrases[302] = "Found";
        reason_phrases[303] = "See Other";
        reason_phrases[304] = "Not Modified";
        reason_phrases[305] = "Use Proxy";
        reason_phrases[307] = "Temporary Redirect";
        reason_phrases[400] = "Bad Request";
        reason_phrases[401] = "Unauthorized";
        reason_phrases[402] = "Payment Required";
        reason_phrases[403] = "Forbidden";
        reason_phrases[404] = "Not Found";
        reason_phrases[405] = "Method Not Allowed";
        reason_phrases[406] = "Not Acceptable";
        reason_phrases[407] = "Proxy Authentication Required";
        reason_phrases[408] = "Request Time-out";
        reason_phrases[409] = "Conflict";
        reason_phrases[410] = "Gone";
        reason_phrases[411] = "Length Required";
        reason_phrases[412] = "Precondition Failed";
        reason_phrases[413] = "Request Entity Too Large";
        reason_phrases[414] = "Request-URI Too Large";
        reason_phrases[415] = "Unsupported Media Type";
        reason_phrases[416] = "Requested range not satisfiable";
        reason_phrases[417] = "Expectation Failed";
        reason_phrases[500] = "Internal Server Error";
        reason_phrases[501] = "Not Implemented";
        reason_phrases[502] = "Bad Gateway";
        reason_phrases[503] = "Service Unavailable";
        reason_phrases[504] = "Gateway Time-out";
        reason_phrases[505] = "HTTP Version not supported";
    }

    //Запуск web-server.
    //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
    void web_server::start() {
        server.start();
    }

    void web_server::stop() {
        return;
    }
}