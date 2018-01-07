#include "client_http_request_handler.h"
using namespace std;

namespace webserver {
    void client_http_request_handler::handle_client_http_request(vector<string> client_message) {
        http_request_parser parser;

        http_request request = parser.parse(client_message);
    }
}