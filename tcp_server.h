#ifndef WEB_TCP_SERVER_H
#define WEB_TCP_SERVER_H

#include <iostream>
#include "client.h"
#include <cstring>
#include <unordered_set>
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
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>

using namespace std;

namespace webserver {
    class tcp_server {
    private:
        const unsigned int allowed_connections_number = 128;

        const unsigned int MAX_EVENTS = 2048;

        unsigned short int PORT;

        int listener_socket;

        int EPoll;

        volatile bool accept_allow;

        struct sockaddr_in server_address{};

        const function<bool(string)>& is_full_message;

        const function<string(string)>& convert_client_message;

        unordered_set<int> socket_list;

        unordered_set<int> closed_socket_list;

        mutex mx;
    public:
        tcp_server(unsigned short int PORT, const function<bool(string)>& is_full_message,
                   const function<string(string)>& convert_client_message);

        void start();

        void stop();

    private:
        void set_nonblock(int sockfd);

        void accept_connections();

        void listen_events();

        void set_client(shared_ptr<client>& cl);

        int find_client_index(unsigned int client_id);
    };
}

#endif //WEB_TCP_SERVER_V2_H
