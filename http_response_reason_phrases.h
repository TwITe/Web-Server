#ifndef WEB_HTTP_RESPONSE_REASON_PHRASES_H
#define WEB_HTTP_RESPONSE_REASON_PHRASES_H

#include <string>
#include <map>>
using namespace std;

namespace webserver {
    class http_reponse_reason_phrases {
    private:
        map<int, string> reason_phrases;
    public:
        http_reponse_reason_phrases();

        string get_suitable_reponse_reason_phrase(int code);
    };
}

#endif //WEB_HTTP_RESPONSE_REASON_PHRASES_H
