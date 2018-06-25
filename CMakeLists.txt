cmake_minimum_required(VERSION 3.9)
project(Web)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pthread")
add_subdirectory(cpr)
include_directories(${CPR_INCLUDE_DIRS})

add_executable(Web web_server.h http_request.h http_header.h request_param.h
        http_request_parser.h http_request.cpp http_request_parser.cpp http_response.cpp http_response.h tcp_server.cpp
        tcp_server.h web_handler.cpp web_handler.h web_server.cpp http_router.cpp http_router.h http_response_builder.cpp http_response_builder.h
        http_parameterized_header_parser.cpp http_parameterized_header_parser.h client.cpp client.h
#        ------------------
#        tests
#        ------------------
#        catch main include
        provide_catch_main.cpp
#        ------------------
        http_request_parser_tests.cpp
        http_parameterized_header_parser_tests.cpp
        http_router_tests.cpp
#
#        serverclient tests
#        ------------------
        web_server_component_tests.cpp
        )

target_link_libraries(Web ${CPR_LIBRARIES})