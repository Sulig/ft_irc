#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <map>
#include <string>

class Client;
class Channel;

struct t_irc
{
    std::map<int, Client*>* clients;          // fd -> Client*
    std::map<std::string,int>* nickToFd;      // nick -> fd  (mantener esto al cambiar NICK)
    std::string serverName;                   // "IRCSERV"
};


void        sendRawFd(int fd, const std::string& raw);
Client*     getClientFd(const t_irc& irc, int fd);
int         getFdByNick(const t_irc& irc, const std::string& nick);
// std::string nickOf(const t_irc& irc, int fd); // cambiado a static y las 2 que usaban esta
std::string userOf(const t_irc& irc, int fd);
void        sendNumeric(const t_irc& irc, int fd, const std::string& code, const std::string& targetNick, const std::string& middle, const std::string& text);
void        chanBroadcast(const t_irc& irc, const Channel* ch, const std::string& cmdLineNoCRLF);
bool        isChannelName(const std::string& s);

#endif
