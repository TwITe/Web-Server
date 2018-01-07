#ifndef WEB_TCP_SERVER_H
#define WEB_TCP_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <thread>
#include <stdexcept>
#include "http_request_parser.h"
#include "http_request.h"
using namespace std;

namespace webserver {
    class tcp_server {
    private:
        unsigned short int PORT;

        int listener_socket;

        struct sockaddr_in server_address{};

        socklen_t socket_length;

        function<vector<string>(char*)> convert_client_message;
    public:
        tcp_server(unsigned short int PORT, function<vector<string>(char*)> convert_client_message);

        void start();

    private:
        void handle_client_http_request(vector<string> client_message);

        void handle_client(int socket);

        void connection_handler(int client_socket);

        void take_requests();
    };
}

#endif //WEB_TCP_SERVER_H
