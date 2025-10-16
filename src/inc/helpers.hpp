#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <map>
#include <string>

class Client;
class Channel;
class Server;

// quitado struct, usamos server por referencia para evitar copias innecesarias

void        sendRawFd(int fd, const std::string& raw);

// Acceso a clientes a través del Server
Client*     getClientFd(Server& serv, int fd);
int         getFdByNick(Server& serv, const std::string& nick);
std::string userOf(Server& serv, int fd);

// Numerics y broadcast
void        sendNumeric(Server& serv, int fd,
                        const std::string& code,
                        const std::string& targetNick,
                        const std::string& middle,
                        const std::string& text);

void        chanBroadcast(Server& serv, const Channel* ch,
                          const std::string& cmdLineNoCRLF);

// Util
bool        isChannelName(const std::string& s);

#endif
