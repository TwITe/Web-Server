#include "web_server.h"
#include "catch.hpp"
using namespace std;

void open_connection(unsigned short int PORT, int& sockfd, struct sockaddr_in& server_address) {
    server_address = {};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (connect(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        cout << "[client] Connection failed" << endl;
    }
    cout << "[client] All setting are done" << endl;
    cout << "[client] Succefully connected to the server" << endl << endl;
    cout << "----------------------------" << endl << endl;
}

TEST_CASE("Get 404 response for request with non-existing web handler","client") {
    unsigned short int PORT = 8080;
    int sockfd;
    struct sockaddr_in server_address{};
    open_connection(PORT, sockfd, server_address);

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
        cout << "[client] Message sending failed" << endl;
    }
    cout << "[client] Message sent to server" << endl;

    char buffer[128000];

    memset(&buffer, 0, 128000);
    read(sockfd, buffer, 128000);

    string received_message = string(buffer);

    string proper_response = "HTTP/1.1 404 Not Found\r\n"
                             "Content-Length: 9\r\n"
                             "\r\n"
                             "Not Found";

    cout << "[client] Server message: " << buffer << endl;
    cout << "============================" << endl << endl;

    REQUIRE(received_message == proper_response);
}

TEST_CASE("Get 200 response for request with existing web handler","client") {
    unsigned short int PORT = 8080;
    int sockfd;
    struct sockaddr_in server_address{};
    open_connection(PORT, sockfd, server_address);

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
        cout << "[client] Message sending failed" << endl;
    }
    cout << "[client] Message sent to server" << endl;

    char buffer[128000];

    memset(&buffer, 0, 128000);
    read(sockfd, buffer, 128000);

    string received_message = string(buffer);

    string proper_response = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: 13\r\n"
                             "\r\n"
                             "Hello, World!";

    cout << "[client] Server message: " << buffer << endl;
    cout << "============================" << endl << endl;

    REQUIRE(received_message == proper_response);
}

//TEST_CASE("Get 400 response for bad request","") {
//    //TODO: дописать тесты
//    unsigned short int PORT = 8080;
//    int sockfd;
//    struct sockaddr_in server_address;
//    open_connection(PORT, sockfd, server_address);
//}