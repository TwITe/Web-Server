#include "tcp_server.h"

namespace webserver {
    tcp_server::tcp_server(unsigned short int PORT, const function<string(string)>& convert_client_message, int allowed_connections_number) :
            PORT(PORT), allowed_connections_number(allowed_connections_number), convert_client_message(convert_client_message) {}

    //TODO: Задачи по tcp серверу:
    // Сделать адекватное закрытие соединений в функции stop(), а также выключение сервера
    // Алгоритм работы сервера:
    // 1. Когда приходит новое подключение с клиента, сначала проверяется хватает ли слотов в массиве, если да, новое подключение (тред) добавляется в массив тредов,
    // иначе добавляется в очередь
    // 2. Клиент обрабатывается в отдельном процессе (треде). Когда на команду recv() приходит 0 байт, то значит подключение на стороне клиента закрыто
    // соответственно, надо высвободить данный тред, чтобы освободился слот под новые. Также требуется закрыть сокет на стороне самого сервера командой close()
    //
    //  TODO: Задачи:
    // 1. Сделать адекватное закрытие соединений в функции stop(), а также выключение сервера
    // 2. Создать класс client
    // 3. Сделать добавление каждому клиенту своего ID, чтобы было понятно какой тред освобождать и какой сокет закрывать

    void tcp_server::start() {
        memset(&server_address, 0, sizeof(server_address));

//        For setsock opt (REUSEADDR)
        int reuse_port = 1;

        listener_socket = socket(AF_INET, SOCK_STREAM, 0);

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        server_address.sin_addr.s_addr = INADDR_ANY;

        //Avoid bind error if the socket was not closed last time;
        setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_port, sizeof(int));

        if (bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
            throw runtime_error("[Server] Binding failed");
        }

        cout << "[Server] Configuring done" << endl;
        cout << "[Server] Server enabled" << endl;

        if (listen(listener_socket, allowed_connections_number) == -1) {
            throw runtime_error("[Server] Listening failed");
        }

        cout << "[Server] Waiting for connection..." << endl;

        take_requests();
    }

    void tcp_server::connection_handler(client* cl) {
        char read_buffer[1024];

        // add client to the clients <vector>
        mx.lock();

        cl->set_id(static_cast<int>(clients.size()));
        clients.push_back(*cl);
        cout << "Client with id " << cl->get_id() << " has added to the clients list" << endl << endl;

        mx.unlock();

        ssize_t size;
        string received_message;
        while (true) {
            memset(&read_buffer, 0, sizeof(read_buffer));

            size = recv(cl->sock, read_buffer, sizeof(read_buffer), 0);

            if (size == 0) {
                // client disconnect
                cout << "Client with id " << cl->get_id() << " has disconnected" << endl;
                close(cl->sock);

                //remove client from clients <vector>
                mx.lock();

                int index = find_client_index(cl);
                clients.erase(clients.begin() + index);
                cout << "Client with id " << cl->get_id() << " was removed from the clients list" << endl << endl;

                mx.unlock();

                delete cl;

                break;
            }
            if (size == -1) {
                cerr << "Error while receiving message from client with id " << cl->get_id() << endl;
            }
            else {
                //message received succesfully
                //TODO: Сделать чтение буферами по 1024 байта

                cout << "[Server] Client's message received" << endl;
                cout << "[Server] Client's message: " << endl;
                cout << "----------------------------" << endl;
                cout << read_buffer;
                cout << "----------------------------" << endl;

                for (unsigned int i = 0; i < size; i++) {
                    received_message.push_back(read_buffer[i]);
                }

                string response_message = convert_client_message(received_message);

                if (send(cl->sock, response_message.c_str(), response_message.size(), 0) == -1) {
                    cout << "[Server] Message sending to client with id " << cl->get_id() << " failed" << endl;
                }

                cout << "[Server] Server's response: " << endl;
                cout << "----------------------------" << endl;
                cout << response_message;
                cout << "----------------------------" << endl;

                cout << "[Server] Message was sent to client" << endl << endl;
                cout << "============================" << endl << endl;
            }

        }
    }

    void tcp_server::take_requests() {
        client* current_client;

        socklen_t cli_size = sizeof(sockaddr_in);

        while (true) {
            current_client = new client;

            current_client->sock = accept(listener_socket, (struct sockaddr *) &server_address, &cli_size);

            if (current_client->sock != -1) {
                cout << "----------------------------" << endl << e ndl;
                cout << "[Server] New connection accepted" << endl << endl;
                cout << "----------------------------" << endl << endl;

                thread handling_thread(&tcp_server::connection_handler, this, current_client);
                handling_thread.detach();
            }
            else {
                cout << "----------------------------" << endl << endl;
                cout << "[Server] Socket accept failed" << endl << endl;
                cout << "----------------------------" << endl << endl;
            }
        }
    }

    void tcp_server::stop() {
        close(listener_socket);
    }

    int tcp_server::find_client_index(client* cl) {
        for (unsigned int i = 0; i < clients.size(); i++) {
            if (clients[i].get_id() == cl->get_id()) {
                return i;
            }
        }
        cerr << "Client with id" << cl->get_id() << " not found" << endl;
        return -1;
    }
}