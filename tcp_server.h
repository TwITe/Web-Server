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
#include <mutex>
#include <stdexcept>
#include "client.h"
#include <queue>
using namespace std;

namespace webserver {
    class tcp_server {
    private:
        unsigned short int PORT;

        const unsigned int allowed_connections_number;

        int listener_socket;

        queue<client*> clients_queue;

        struct sockaddr_in server_address{};

        const function<string(string)>& convert_client_message;

        vector<client> clients;

        mutex mx;
    public:
        tcp_server(unsigned short int PORT, const function<string(string)>& convert_client_message, int allowed_connections_number);

        void start();

        void stop();
    private:
        void connection_handler(client* cl);

        void take_requests();

        void handle_awaiting_clients();

        int find_client_index(client* cl);
    };
}

#endif //WEB_TCP_SERVER_H
