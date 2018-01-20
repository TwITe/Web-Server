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
using namespace std;

namespace webserver {
    class tcp_server {
    private:
        unsigned short int PORT;

        int listener_socket;

        struct sockaddr_in server_address{};

        socklen_t socket_length;

        function<string(char*)> convert_client_message;
    public:
        tcp_server(unsigned short int PORT, function<string(char*)>& convert_client_message);

        void start();

        void stop();
    private:
        void connection_handler(int client_socket);

        void take_requests();
    };
}

#endif //WEB_TCP_SERVER_H
