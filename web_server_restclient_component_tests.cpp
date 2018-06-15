#include "restclient-cpp/restclient.h"
//#include "catch.hpp"
//#include "web_server.h"
using namespace std;

//TEST_CASE("Web-Server_SendSimpleRequest_GetResponse","Component Tests") {
//    RestClient::Response r = RestClient::get("http:localhost:8080");
//
//
//}

int main() {
    RestClient::Response r = RestClient::get("http:localhost:8080");

    return 0;
}