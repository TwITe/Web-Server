#include "web_server.h"
#include "catch.hpp"
using namespace std;

unsigned short int PORT = 8080;

void open_connection(int& sockfd, struct sockaddr_in& server_address) {
    server_address = {};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (connect(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        cout << "[Client] Connection failed" << endl;
    }
    cout << "[Client] All setting are done" << endl;
    cout << "[Client] Succefully connected to the server" << endl << endl;
    cout << "----------------------------" << endl << endl;
}

TEST_CASE("Get 404 response on request with non-existing web handler","Client") {
    int sockfd;
    struct sockaddr_in server_address{};
    open_connection(sockfd, server_address);

    string message = "GET /tutorials/other/top-20-mysql-best-practices/ HTTP/1.1\r\n"
                     "Host: net.tutsplus.com\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)\r\n"
                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8*\r\n"
                     "Accept-Language: en-us,en;q=0.5\r\n"
                     "Accept-Encoding: gzip,deflate\r\n"
                     "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
                     "Keep-Alive: 300\r\n"
                     "Connection: keep-alive\n"
                     "Cookie: PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120\r\n"
                     "Pragma: no-cache\r\n"
                     "Cache-Control: no-cache\r\n"
                     "\r\n";
    if (write(sockfd, message.c_str(), message.size()) == -1) {
        cerr << "[Client] Message sending failed" << endl;
    }
    else {
        cout << "[Client] Message was sent to server" << endl;
    }

    char buffer[128000];

    memset(&buffer, 0, 128000);
    read(sockfd, buffer, 128000);

    string received_message = string(buffer);

    string proper_response = "HTTP/1.1 404 Not Found\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: 9\r\n"
                             "\r\n"
                             "Not Found";

    cout << "[Client] Server's message: " << endl;
    cout << "----------------------------" << endl;
    cout << buffer << endl;
    cout << "----------------------------" << endl;

    close(sockfd);

    REQUIRE(received_message == proper_response);
}

TEST_CASE("Get 200 response on request with existing web handler","Client") {
    int sockfd;
    struct sockaddr_in server_address{};
    open_connection(sockfd, server_address);

    string message = "GET /index HTTP/1.1\r\n"
                     "Host: net.tutsplus.com\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)\r\n"
                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8*\r\n"
                     "Accept-Language: en-us,en;q=0.5\r\n"
                     "Accept-Encoding: gzip,deflate\r\n"
                     "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
                     "Keep-Alive: 300\r\n"
                     "Connection: keep-alive\n"
                     "Cookie: PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120\r\n"
                     "Pragma: no-cache\r\n"
                     "Cache-Control: no-cache\r\n"
                     "\r\n";
    if (write(sockfd, message.c_str(), message.size()) == -1) {
        cerr << "[Client] Message sending failed" << endl;
    }
    else {
        cout << "[Client] Message was sent to server" << endl;
    }

    char buffer[128000];

    memset(&buffer, 0, 128000);
    read(sockfd, buffer, 128000);

    string received_message = string(buffer);

    string proper_response = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: 13\r\n"
                             "\r\n"
                             "Hello, World!";

    cout << "[Client] Server's message: " << endl;
    cout << "----------------------------" << endl;
    cout << buffer << endl;
    cout << "----------------------------" << endl;

    close(sockfd);

    REQUIRE(received_message == proper_response);
}

TEST_CASE("Send multiple messages from single client", "Client") {
    int sockfd;
    struct sockaddr_in server_address{};
    open_connection(sockfd, server_address);

    string message = "GET /index HTTP/1.1\r\n"
                     "Host: net.tutsplus.com\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)\r\n"
                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8*\r\n"
                     "Accept-Language: en-us,en;q=0.5\r\n"
                     "Accept-Encoding: gzip,deflate\r\n"
                     "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
                     "Keep-Alive: 300\r\n"
                     "Connection: keep-alive\n"
                     "Cookie: PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120\r\n"
                     "Pragma: no-cache\r\n"
                     "Cache-Control: no-cache\r\n"
                     "\r\n";
    if (write(sockfd, message.c_str(), message.size()) == -1) {
        cerr << "[Client] Message sending failed" << endl;
    }
    else {
        cout << "[Client] Message was sent to server" << endl;
    }

    char buffer[128000];

    memset(&buffer, 0, 128000);
    read(sockfd, buffer, 128000);

    string received_message = string(buffer);

    string proper_response = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: 13\r\n"
                             "\r\n"
                             "Hello, World!";

    cout << "[Client] Server's message: " << endl;
    cout << "----------------------------" << endl;
    cout << buffer << endl;
    cout << "----------------------------" << endl;

    REQUIRE(received_message == proper_response);

    //--------
    message =        "GET /tutorials/other/top-20-mysql-best-practices/ HTTP/1.1\r\n"
                     "Host: net.tutsplus.com\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)\r\n"
                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8*\r\n"
                     "Accept-Language: en-us,en;q=0.5\r\n"
                     "Accept-Encoding: gzip,deflate\r\n"
                     "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
                     "Keep-Alive: 300\r\n"
                     "Connection: keep-alive\n"
                     "Cookie: PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120\r\n"
                     "Pragma: no-cache\r\n"
                     "Cache-Control: no-cache\r\n"
                     "\r\n";
    if (write(sockfd, message.c_str(), message.size()) == -1) {
        cerr << "[Client] Message sending failed" << endl;
    }
    else {
        cout << "[Client] Message was sent to server" << endl;
    }

    memset(&buffer, 0, 128000);
    read(sockfd, buffer, 128000);

    received_message = string(buffer);

    proper_response = "HTTP/1.1 404 Not Found\r\n"
                      "Content-Type: text/plain\r\n"
                             "Content-Length: 9\r\n"
                             "\r\n"
                             "Not Found";

    cout << "[Client] Server's message: " << endl;
    cout << "----------------------------" << endl;
    cout << buffer << endl;
    cout << "----------------------------" << endl;

    REQUIRE(received_message == proper_response);

    close(sockfd);
}

TEST_CASE("Connect Multiple Clients", "Client") {
    //allowed_connections value set to 1
    int sockfd_1;

    struct sockaddr_in server_address_1{};
    open_connection(sockfd_1, server_address_1);

    string message = "GET /index HTTP/1.1\r\n"
                     "Host: net.tutsplus.com\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)\r\n"
                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8*\r\n"
                     "Accept-Language: en-us,en;q=0.5\r\n"
                     "Accept-Encoding: gzip,deflate\r\n"
                     "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
                     "Keep-Alive: 300\r\n"
                     "Connection: keep-alive\n"
                     "Cookie: PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120\r\n"
                     "Pragma: no-cache\r\n"
                     "Cache-Control: no-cache\r\n"
                     "\r\n";
    if (write(sockfd_1, message.c_str(), message.size()) == -1) {
        cout << "[Client] Message sending failed" << endl;
    }
    cout << "[Client] Message was sent to server" << endl;

    char buffer[128000];

    memset(&buffer, 0, 128000);
    read(sockfd_1, buffer, 128000);

    string received_message = string(buffer);

    string proper_response = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: 13\r\n"
                             "\r\n"
                             "Hello, World!";

    cout << "[Client] Server's message: " << endl;
    cout << "----------------------------" << endl;
    cout << buffer << endl;
    cout << "----------------------------" << endl;

    REQUIRE(received_message == proper_response);

    //-------
    int sockfd_2;
    struct sockaddr_in server_address_2{};

    open_connection(sockfd_2, server_address_2);

    message =        "GET /tutorials/other/top-20-mysql-best-practices/ HTTP/1.1\r\n"
                     "Host: net.tutsplus.com\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)\r\n"
                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8*\r\n"
                     "Accept-Language: en-us,en;q=0.5\r\n"
                     "Accept-Encoding: gzip,deflate\r\n"
                     "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
                     "Keep-Alive: 300\r\n"
                     "Connection: keep-alive\n"
                     "Cookie: PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120\r\n"
                     "Pragma: no-cache\r\n"
                     "Cache-Control: no-cache\r\n"
                     "\r\n";
    if (write(sockfd_2, message.c_str(), message.size()) == -1) {
        cout << "[Client] Message sending failed" << endl;
    }
    cout << "[Client] Message was sent to server" << endl;

    //close 1st connection to free slot for 2nd client
    close(sockfd_1);

    memset(&buffer, 0, 128000);
    read(sockfd_2, buffer, 128000);

    received_message = string(buffer);

    proper_response = "HTTP/1.1 404 Not Found\r\n"
                      "Content-Type: text/plain\r\n"
                      "Content-Length: 9\r\n"
                      "\r\n"
                      "Not Found";

    cout << "[Client] Server's message: " << endl;
    cout << "----------------------------" << endl;
    cout << buffer << endl;
    cout << "----------------------------" << endl;

    REQUIRE(received_message == proper_response);

    close(sockfd_2);
}