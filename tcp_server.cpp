#include "tcp_server.h"

namespace webserver {
    tcp_server::tcp_server(unsigned short int PORT, const function<string(string)>& convert_client_message, unsigned int allowed_connections_number) :
            PORT(PORT), allowed_connections_number(allowed_connections_number), convert_client_message(convert_client_message) {}

    void tcp_server::start() {
        memset(&server_address, 0, sizeof(server_address));

        listener_socket = socket(AF_INET, SOCK_STREAM, 0);

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        server_address.sin_addr.s_addr = INADDR_ANY;

        //For setsock opt (REUSEADDR)
        int reuse_port = 1;

        //Avoid bind error if the socket was not closed last time;
        setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_port, sizeof(int));

        if (bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
            throw runtime_error("[Server] Binding failed");
        }

        cout << "[Server] Configuring was done" << endl;
        cout << "[Server] Server was enabled" << endl;

        if (listen(listener_socket, allowed_connections_number) == -1) {
            throw runtime_error("[Server] Socket listening failed");
        }

        cout << "[Server] Waiting for connections on port " << PORT << "..." << endl;

        take_requests();
    }

    int tcp_server::find_client_index(client* cl) {
        for (unsigned int index = 0; index < clients.size(); index++) {
            if (clients[index].get_id() == cl->get_id()) {
                return index;
            }
        }
        cerr << "Client with id" << cl->get_id() << " not found" << endl;
        return -1;
    }

    void tcp_server::connection_handler(client* cl) {
        // add client to the clients <vector>
        mx.lock();

        cl->set_id(static_cast<int>(clients.size()));
        clients.push_back(*cl);
        cout << "New client with id " << cl->get_id() << " was added to the clients list" << endl << endl;

        mx.unlock();

        char read_buffer[1024];
        ssize_t message_size;
        string received_message;
        while (true) {
            memset(&read_buffer, 0, sizeof(read_buffer));
            received_message.clear();

            message_size = recv(cl->sock, read_buffer, sizeof(read_buffer), 0);

            if (message_size == 0) {
                // client disconnect
                cout << "Client with id " << cl->get_id() << " has disconnected" << endl;
                close(cl->sock);

                //remove client from the clients <vector>
                mx.lock();

                int client_index = find_client_index(cl);
                clients.erase(clients.begin() + client_index);
                cout << "Client with id " << cl->get_id() << " was removed from the clients list" << endl;

                mx.unlock();

                delete cl;

                // place was freed, handle clients in queue
                handle_awaiting_clients();

                break;
            }
            if (message_size == -1) {
                cerr << "Error while receiving message from client with id " << cl->get_id() << endl;
            }
            else {
                //message received succesfully
                //TODO: Сделать чтение буферами по 1024 байта

                cout << "[Server] Client's message received" << endl;
                cout << "[Server] Client's message: " << endl;
                cout << "----------------------------" << endl;
                cout << read_buffer << endl;
                cout << "----------------------------" << endl;

                for (unsigned int i = 0; i < message_size; i++) {
                    received_message.push_back(read_buffer[i]);
                }

                string response_message = convert_client_message(received_message);

                if (send(cl->sock, response_message.c_str(), response_message.size(), 0) == -1) {
                    cout << "[Server] Message sending to client with id " << cl->get_id() << " failed" << endl;
                }

                cout << "[Server] Server's response: " << endl;
                cout << "----------------------------" << endl;
                cout << response_message << endl;
                cout << "----------------------------" << endl << endl;

                cout << "[Server] Message was sent to client" << endl << endl;
                cout << "============================" << endl << endl;
            }
        }
    }

    void tcp_server::handle_awaiting_clients() {
        mx.lock();
        if (!clients_queue.empty()) {

            cout << "[Server] Handling awaiting clients" << endl << endl;

            client* cl = clients_queue.front();

            clients_queue.pop();

            thread handling_thread(&tcp_server::connection_handler, this, cl);
            handling_thread.detach();
        }
        mx.unlock();
    }

    void tcp_server::take_requests() {
        client* current_client;

        socklen_t cli_size = sizeof(sockaddr_in);

        while (true) {
            current_client = new client();

            current_client->sock = accept(listener_socket, (struct sockaddr *) &server_address, &cli_size);

            if (current_client->sock != -1) {
                cout << "----------------------------" << endl << endl;
                cout << "[Server] New connection accepted" << endl << endl;
                cout << "----------------------------" << endl << endl;


                if (clients.size() < allowed_connections_number) {
                    thread handling_thread(&tcp_server::connection_handler, this, current_client);
                    handling_thread.detach();
                }
                else {
                    cout << "[Server] No free slots. Your place in queue is " << clients_queue.size() + 1 << ". Please stand by..." << endl;
                    clients_queue.push(current_client);
                }
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
}