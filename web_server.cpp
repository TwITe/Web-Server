#include "web_server.h"

namespace webserver {
    web_server::web_server(unsigned short int port, const vector<web_handler>& handlers) : PORT(port), handlers(handlers) {}

    //Запуск web-server.
    //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
    void web_server::start() {
        tcp_server server(PORT, convert_client_message);

        server.start();
    }

    void web_server::stop() {
        return;
    }
}