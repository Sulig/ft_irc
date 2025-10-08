#include "inc/channel_more.hpp"

// Helper local (envía una línea al fd; la línea debe terminar en \r\n)
void sendRawFd(int fd, const std::string& raw)
{
    ::send(fd, raw.c_str(), raw.size(), 0);
}

