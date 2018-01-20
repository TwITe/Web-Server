#include "tcp_server.h"

namespace webserver {
    tcp_server::tcp_server(unsigned short int PORT, const function<string(char*)>& convert_client_message) : PORT(PORT), convert_client_message(convert_client_message) {}

    void tcp_server::start() {
        memset(&server_address, 0, sizeof(server_address));

        listener_socket = socket(AF_INET, SOCK_STREAM, 0);

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        if (inet_aton("127.0.0.1", &server_address.sin_addr) == 0) {
            throw runtime_error("[Server] Invalid IP address");
        }

        if (bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
            throw runtime_error("[Server] Binding failed");
        }

        cout << "[Server] Configuring done" << endl;
        cout << "[Server] Server enabled" << endl;

        if (listen(listener_socket, 30) == -1) {
            throw runtime_error("[Server] Listening failed");
        }

        cout << "[Server] Waiting for connection..." << endl;

        take_requests();
    }

    void tcp_server::connection_handler(int socket) {
        char read_buffer[128000];
        memset(&read_buffer, 0, sizeof(read_buffer));
        size_t message_size = 0;

        while ((message_size = static_cast<size_t>(recv(socket, read_buffer, sizeof(read_buffer), 0))) > 0) {
            cout << "[Server] Client message accepted" << endl;
            cout << "[Server] Client message: " << endl << read_buffer << endl;

            string converted_message_for_send = convert_client_message(read_buffer);

            if (send(socket, converted_message_for_send.c_str(), converted_message_for_send.size(), 0) == -1) {
                cout << "[Server] Message send failure" << endl;
            }

            cout << converted_message_for_send << endl;

//            if (send(socket, read_buffer, message_size, 0) == -1) {
//                cout << "[Client] Message sending failed" << endl;
//                return;
//            }
            cout << "[Server] Message sent to client" << endl << endl;
            cout << "============================" << endl << endl;

            memset(&read_buffer, 0, 128000);
        }
    }

    void tcp_server::take_requests() {
        int client_socket;
        while (true) {
            client_socket = accept(listener_socket, (struct sockaddr*)& server_address, &socket_length);
            cout << "----------------------------" << endl << endl;
            cout << "[Server] New connection accepted" << endl << endl;
            cout << "----------------------------" << endl << endl;

            thread handling_thread(&tcp_server::connection_handler, this, client_socket);
            handling_thread.detach();
        }
    }

    void tcp_server::stop() {
        return;
    }
}