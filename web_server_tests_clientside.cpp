#include "web_server.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("Check base read/send functions","Client") {
    unsigned short int PORT = 8080;
    int sockfd;
    struct sockaddr_in server_address{};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_address, '0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    if (inet_aton("127.0.0.1", &server_address.sin_addr) == 0) {
        cout << "[Server] Invalid IP address" << endl;
    }

    if (connect(sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        cout << "[Client] Connection failed" << endl;
    }
    cout << "[Client] All setting are done" << endl;
    cout << "[Client] Succefully connected to server" << endl << endl;
    cout << "----------------------------" << endl << endl;

    string message = "GET /tutorials/other/top-20-mysql-best-practices/ HTTP/1.1\n"
            "Host: net.tutsplus.com\n"
            "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8*\n"
            "Accept-Language: en-us,en;q=0.5\n"
            "Accept-Encoding: gzip,deflate\n"
            "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\n"
            "Keep-Alive: 300\n"
            "Connection: keep-alive\n"
            "Cookie: PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120\n"
            "Pragma: no-cache\n"
            "Cache-Control: no-cache\n"
            "\n";
    if (write(sockfd, message.c_str(), message.size()) == -1) {
        cout << "[Client] Message sending failed" << endl;
    }
    cout << "[Client] Message sent to server" << endl;

    char buffer[128000];

    memset(&buffer, 0, 128000);
    read(sockfd, buffer, 128000);

    string received_message = string(buffer);

    REQUIRE(received_message == message);

    cout << "[Client] Server message: " << buffer << endl;
    cout << "============================" << endl << endl;
}