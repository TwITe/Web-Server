#include "tcp_server.h"

namespace webserver {
    tcp_server::tcp_server(unsigned short int PORT, function<vector<string>(char*)> convert_client_message) : PORT(PORT), convert_client_message(convert_client_message) {}

    bool tcp_server::start() {
        memset(&server_address, 0, sizeof(server_address));

        listener_socket = socket(AF_INET, SOCK_STREAM, 0);

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        if (inet_aton("127.0.0.1", &server_address.sin_addr) == 0) {
            cout << "[Server] Invalid IP address" << endl;
            return false;
        }

        if (bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
            cout << "[Server] Binding failed" << endl;
            return false;
        }

        if (listen(listener_socket, 30) == -1) {
            cout << "[Server] Listening failed" << endl;
            return false;
        }

        cout << "[Server] All setting are done" << endl;
        cout << "[Server] Server enabled" << endl;

        take_requests();
    }

    void tcp_server::handle_client_http_request(vector<string> client_message) {
        http_request_parser parser;

        http_request request = parser.parse(client_message);
    }

    void tcp_server::handle_client(int socket) {
        char read_buffer[128000];

        while (true) {
            recv(socket, read_buffer, sizeof(read_buffer), 0);
            cout << "[Server] Client message accepted" << endl;
            cout << "[Server] Client message: " << read_buffer << endl;

            handle_client_http_request(convert_client_message(read_buffer));

            memset(&read_buffer, 0, sizeof(read_buffer));
        }
    };

    void tcp_server::connection_handler(int client_socket) {
        cout << "----------------------------" << endl << endl;
        cout << "[Server] Connection accepted" << endl << endl;
        cout << "----------------------------" << endl << endl;

        handle_client(client_socket);
    }

    void tcp_server::take_requests() {
        for (int i = 0; i > 0; i++) {
            int client_socket;
            client_socket = accept(listener_socket, (struct sockaddr*)& server_address, &socket_length);

            cout << "----------------------------" << endl << endl;
            cout << "[Server] Connection accepted" << endl << endl;
            cout << "----------------------------" << endl << endl;

            thread handling_thread(&tcp_server::connection_handler, this, client_socket);
            handling_thread.detach();
        }
    }
}