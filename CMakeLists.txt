cmake_minimum_required(VERSION 3.9)
project(Web)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pthread")

add_executable(Web web_server.h http_request.h http_header.h request_param.h
        http_request_parser.h http_request.cpp http_request_parser.cpp http_response.cpp http_response.h tcp_server.cpp
        tcp_server.h web_handler.cpp web_handler.h web_server.cpp http_router.cpp http_router.h

#        tests
#        ------------------
#        catch main include
        provide_catch_main.cpp
#        ------------------
#        request_parser_tests.cpp
#        http_router_tests.cpp

#        web server tests
#        run server firstly and only then clientside tests, do not running server again
#        web server launch
#        ------------------
#        web_server_run.cpp
#        ------------------
#        serverclient tests
#        ------------------
        web_server_tests_clientside.cpp
        )