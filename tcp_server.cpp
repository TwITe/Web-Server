#include "tcp_server.h"

// TODO: сделать single-threaded non-blocking I/O сервер, использущий epoll в режиме edge-triggered

namespace webserver {
    tcp_server::tcp_server(unsigned short int PORT, const function<bool(string)>& is_full_message,
                           const function<string(string)>& convert_client_message) :
            PORT(PORT), is_full_message(is_full_message),
            convert_client_message(convert_client_message) {
        accept_allow = true;
    }

    void tcp_server::set_nonblock(int sockfd) {
        const int flags = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    }

    void tcp_server::start() {
        memset(&server_address, 0, sizeof(server_address));

        listener_socket = socket(AF_INET, SOCK_STREAM, 0);

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);

        int reuse_port = 1;
        setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_port, sizeof(reuse_port));

        if (bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
            throw runtime_error("[Server] Binding failed");
        }
        set_nonblock(listener_socket);

        if (listen(listener_socket, allowed_connections_number) == -1) {
            throw runtime_error("[Server] Listening failed");
        }

        cout << "[Server] Configuring was done" << endl;
        cout << "[Server] Server was enabled" << endl;
        cout << "[Server] Waiting for connections on port " << PORT << "..." << endl;

        EPoll = epoll_create1(0);

        thread accept_conns_thread(accept_connections);
        accept_conns_thread.detach();

        thread listen_events_thread(listen_events);
        listen_events_thread.detach();
    }

    int tcp_server::find_client_index(unsigned int client_id) {
        for (unsigned int index = 0; index < clients.size(); index++) {
            if (clients[index].get_id() == client_id) {
                return index;
            }
        }

        return -1;
    }

    void tcp_server::set_client(shared_ptr<client>& cl) {
        // TODO: выяснить, нужен ли тут mutex
        cl->set_id(static_cast<int>(clients.size()));

        mx.lock();

        clients.push_back(*cl);

        mx.unlock();

        cout << "New client [ID " << cl->get_id() << "] has been added to the clients list" << endl << endl;

        struct epoll_event client_event;
        client_event.data.fd = cl->sock;
        client_event.data.u32 = static_cast<uint32_t>(cl->get_id());
        client_event.events = EPOLLIN;

        epoll_ctl(EPoll, EPOLL_CTL_ADD, cl->sock, &client_event);
    }

    void tcp_server::accept_connections() {
        shared_ptr<client> current_client;

        while (accept_allow) {
            current_client = make_shared<client>();

            current_client->sock = accept4(listener_socket, nullptr, nullptr, SOCK_NONBLOCK);

            if (current_client->sock != -1 && accept_allow) {
                cout << "----------------------------" << endl << endl;
                cout << "[Server] New connection has been accepted" << endl << endl;
                cout << "----------------------------" << endl << endl;

                set_client(current_client);
            } else {
                cerr << "----------------------------" << endl << endl;
                cerr << "[Server] Socket accept failure" << endl << endl;
                cerr << strerror(errno) << endl;
                cerr << "----------------------------" << endl << endl;
            }
        }
    }

    void tcp_server::listen_events() {
        while (true) {
            struct epoll_event events[MAX_EVENTS];
            int active_events_num = epoll_wait(EPoll, events, MAX_EVENTS, -1);
            if (active_events_num == -1) {
                cerr << "Error occurred while checking for an I/O events. Error message: " << strerror(errno) << endl;
            }

            for (int current_event = 0; current_event < active_events_num; current_event++) {
                char readBuffer[1024];
                memset(&readBuffer, '\0', sizeof(readBuffer));

                int current_socket = events[current_event].data.fd;
                unsigned int current_socket_id = events[current_event].data.u32;

                ssize_t recvSize = recv(current_socket, &readBuffer, sizeof(readBuffer) - 1, MSG_NOSIGNAL);

                if (recvSize == 0) {
                    cout << "Client with id " << current_socket_id << " has been disconnected" << endl;

                    shutdown(current_socket, SHUT_RDWR);

                    if (close(current_socket) != -1) {
                        cout << "Server side socket related to the client [ID " << current_socket_id
                             << "] has been closed" << endl;
                    } else {
                        cerr << "Unable to close server side socket related to the client [ID " << current_socket_id <<
                             "]. Error message: " << strerror(errno) << endl;
                    }

                    // TODO: придумать, как удалять сокеты из списка, не блокируя поток
                    mx.lock();

                    int client_index = find_client_index(current_socket_id);
                    if (client_index != -1) {
                        clients.erase(clients.begin() + client_index);
                        cout << "Client [ID " << current_socket_id << "] has been removed from the clients vector" << endl;
                    } else {
                        cout << "Client was not found in the clients vector" << endl;
                    }

                    mx.unlock();
                } else if (recvSize > 0) {
                    char read_buffer[1024];
                    string received_message;

                    cout << "[Server] Client's message has been received" << endl;
                    cout << "[Server] Client's message: " << endl;
                    cout << "----------------------------" << endl;
                    cout << read_buffer << endl;
                    cout << "----------------------------" << endl;

                    received_message.append(read_buffer, static_cast<unsigned long>(recvSize));

                    if (is_full_message(received_message)) {
                        string response = convert_client_message(received_message);

                        cout << "[Server] Server's response: " << endl;
                        cout << "----------------------------" << endl;
                        cout << response << endl;
                        cout << "----------------------------" << endl << endl;

                        if (send(current_socket, response.c_str(), response.size(), MSG_NOSIGNAL) != -1) {
                            cout << "[Server] Message has been sent to client [ID " << current_socket_id << "]" << endl;
                            cout << "============================" << endl << endl;
                        } else {
                            cerr << "[Server] Message sending to client [ID " << current_socket_id << "] failed" << endl;
                            cerr << "============================" << endl << endl;
                        }

                        received_message.clear();
                    }
                } else {
                    cerr << "Error while receiving message from client [ID " << events[current_event].data.u32
                         << "]. Error message: " << strerror(errno) << endl;
                }
            }
        }
    }

    void tcp_server::stop() {
        accept_allow = false;

        cout << "[Server Stop] Server terminating has started" << endl;

        if (close(listener_socket) != -1) {
            cout << "[Server Stop] Listener socket has been closed" << endl;
        } else {
            cerr << "[Server Stop] Unable to close listener socket"
                 << ". Error message: " << strerror(errno) << endl;
        }

        mx.lock();
        for (auto& cl : clients) {
            // client disconnect
            if (close(cl.sock) != -1) {
                cout << "[Server Stop] Connection with client [ID " << cl.get_id() << "] has been closed" << endl;
            } else {
                cerr << "[Server Stop] Unable to close connection with client [ID " << cl.get_id()
                     << "]. Error message: " << strerror(errno) << endl;
            }
        }
        mx.unlock();

        cout << "[Server Stop] Server has been terminated" << endl;
    }
}