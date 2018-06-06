#ifndef WEB_HTTP_REQUEST_PARSER_POST_H
#define WEB_HTTP_REQUEST_PARSER_POST_H

#include "http_request.h"
#include "http_request_parser.h"

namespace webserver {
    class http_request_parser_post : http_request_parser {
    private:
        vector<string> get_request_body(const vector<string>& raw_request);

        class Request_body {
        public:
            virtual void parse_request_body(http_request &post_request, const vector<string> &raw_request_body,
                                            const ContentType &obj) = 0;
        };

        class UrlencodedBody : Request_body {
        public:
            void parse_request_body(http_request &post_request, const vector<string> &raw_request_body,
                                    const ContentType &obj) override;
        };

        class FormDataBody : Request_body {
        private:
            string get_request_body_boundary(const ContentType &obj);
        public:
            void parse_request_body(http_request &post_request, const vector<string> &raw_request_body,
                                    const ContentType &obj) override;
        };

        void detect_request_body_content_type(Request_body *post_request_body, const ContentType &content_type);
    public:
        void parse_post_request(http_request& post_request, const vector<string> &raw_request);
    };
}

#endif //WEB_HTTP_REQUEST_PARSER_POST_H