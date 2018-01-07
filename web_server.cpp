#include "web_server.h"

namespace webserver {
       web_server::web_server(unsigned short int port, vector<web_handler> handlers) : PORT(port), handlers(handlers) {}

        function<vector <string>(char*)> web_server::convert_client_message = [&](char* request_char_buffer) {
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
        void web_server::start() {
            tcp_server server(PORT, convert_client_message);
            if (!server.start()) {
                throw runtime_error("Failed to start TCP Server");
            }
        };

        void web_server::stop() {
            return;
        }
    };
}