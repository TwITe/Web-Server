#ifndef WEB_TCP_SERVER_H
#define WEB_TCP_SERVER_H

#include <iostream>
#include "client.h"
#include <cstring>
#include <unistd.h>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <cerrno>
#include <stdexcept>
#include <queue>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

namespace webserver {
    class tcp_server {
    private:
        const unsigned int allowed_connections_number = 128;

        unsigned short int PORT;

        int listener_socket;

        volatile bool accept_allow;

        struct sockaddr_in server_address{};

        const function<bool(string)>& is_full_message;

        const function<string(string)>& convert_client_message;

        vector<client> clients;

        mutex mx;
    public:
        tcp_server(unsigned short int PORT, const function<bool(string)>& is_full_message,
                   const function<string(string)>& convert_client_message);

        void start();

        void stop();

    private:
        void connection_handler(const shared_ptr<client>& cl);

        void accept_connections();

        int find_client_index(const shared_ptr<client>& cl);
    };
}

#endif //WEB_TCP_SERVER_H
