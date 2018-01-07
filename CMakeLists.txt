cmake_minimum_required(VERSION 3.9)
project(Web)

set(CMAKE_CXX_STANDARD 14)
#[[set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")]]

add_executable(Web web_server.h tests.cpp http_request.h http_header.h request_param.h http_request_parser.h
        http_request.cpp http_request_parser.cpp http_response.cpp http_response.h tcp_server.cpp tcp_server.h web_handler.cpp web_handler.h web_server.cpp)