#include "inc/channel.hpp"
#include "inc/helpers.hpp"

// :: antes indica: “usa la función global send del sistema”, send devuelve ssize_t bytes enviados
void sendRawFd(int fd, const std::string& raw)
{
    ::send(fd, raw.c_str(), raw.size(), 0);
}

Client* getClientFd(const t_irc& irc, int fd)
{
    if (!irc.clients)
        return 0;
    std::map<int, Client*>::iterator iter = irc.clients->find(fd);
    return ((iter == irc.clients->end()) ? 0 : iter->second);
}

int getFdByNick(const t_irc& irc, const std::string& nick)
{
    if (!irc.nickToFd)
        return -1;
    std::map<std::string,int>::iterator iter = irc.nickToFd->find(nick);
    return ((iter == irc.nickToFd->end()) ? -1 : iter->second);
}

std::string userOf(const t_irc& irc, int fd)
{
    Client* c = getClientFd(irc, fd);
    // Si no tienes getter de user aún, devuelve "-"
    return (c ? "-" : "-");
}

void sendNumeric(const t_irc& irc, int fd, const std::string& code,
                        const std::string& targetNick,
                        const std::string& middle,
                        const std::string& text)
{
    std::string raw = ":" + irc.serverName + " " + code + " " + targetNick;
    if (!middle.empty())
        raw += " " + middle;
    raw += " :" + text + "\r\n";
    sendRawFd(fd, raw);
}

// Broadcast dentro de un canal
void chanBroadcast(const t_irc& irc, const Channel* ch, const std::string& cmdLineNoCRLF)
{
    // Necesitas acceso a los miembros: expón una forma si no la tienes.
    // Como no hay getter directo, usa Channel::broadcast([...]) que ya tienes:
    // construye un raw ya con prefijo fuera (en ese caso usa ch->broadcast(..., rawCompleto))
    // Aquí lo haremos con el broadcast existente del Channel:
    (void)irc; (void)ch; (void)cmdLineNoCRLF;
    // Si tu Channel::broadcast ya arma el prefijo, llama a ch->broadcast( *irc.clients, rawCompleto );
}

// Helpers de validación
bool isChannelName(const std::string& s)
{
    return !s.empty() && s[0] == '#';
}
