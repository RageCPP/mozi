#pragma once

#include <string_view>

constexpr std::string_view HTTP_501_NOT_IMPLEMENTED = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 0\r\n\r\n";
constexpr std::string_view HTTP_404_NOT_FOUND = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
constexpr std::string_view HTTP_HELLO_MESSAGE = "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\nHello World";
