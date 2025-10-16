# include "inc/channel.hpp"
# include "inc/helpers.hpp"
# include "inc/Server.hpp"
# include "inc/Client.hpp"

// :: antes indica: “usa la función global send del sistema”, send devuelve ssize_t bytes enviados
void sendRawFd(int fd, const std::string& raw)
{
    ::send(fd, raw.c_str(), raw.size(), 0);
}

Client* getClientFd(Server& serv, int fd) // existe en server, pero ya lo tengo
{
    return serv.getClient(fd); // NULL si no existe
}

int getFdByNick(Server& serv, const std::string& nick)
{
    std::map<int, Client*> clients = serv.getClients(); // copia (no podemos tomar referencia)
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second && it->second->getNick() == nick)
            return it->first;
    }
    return -1;
}

std::string userOf(Server& serv, int fd)
{
    Client* c = serv.getClient(fd);
    // Si todavía no tenéis “username/realname” en Client, devolvemos "-".
    (void)c;
    return "-";
}

void sendNumeric(Server& serv, int fd,
                 const std::string& code,
                 const std::string& targetNick,
                 const std::string& middle,
                 const std::string& text)
{
    (void)serv; // no hace falta
    std::string raw = ":" + std::string(SERVER_NAME) + " " + code + " " + (targetNick.empty() ? "*" : targetNick);
    if (!middle.empty())
        raw += " " + middle;
    raw += " :" + text + "\r\n";
    sendRawFd(fd, raw);
}

void chanBroadcast(Server& serv, const Channel* ch, const std::string& cmdLineNoCRLF)
{
    if (!ch)
        return;

    // Asegura CRLF
    std::string raw = cmdLineNoCRLF;
    if (raw.size() < 2 || raw.substr(raw.size()-2) != "\r\n")
        raw += "\r\n";

    // Channel::broadcast(const map<int,Client*>&, const string&)
    ch->broadcast(serv.getClients(), raw);
}

bool isChannelName(const std::string& s)
{
    return !s.empty() && s[0] == '#';
}
