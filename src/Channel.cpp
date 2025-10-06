#include "./Channel.hpp"

const std::string& Channel::name() const
{
    return _name;  // string privado de Channel
}

const std::string& Channel::topic() const
{
    return _topic;
}

std::size_t Channel::getSize() const
{
    return _members.size();
}

bool Channel::has(Client* c) const
{
    // for = mientras no hayamos llegado al final
    for (std::vector<Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
    {
        if (*it == c) return true;   // mismo puntero => mismo cliente
    }
    return false;
}

void Channel::setTopic(const std::string& t)
{
    _topic = t; // Solo cambia el dato; el broadcast del TOPIC lo hará el comando TOPIC
}

void Channel::add(Client* c)
{
    if (!c)
        return;  // ignora puntero nulo
    if (has(c))
        return;  // evita duplicados
    _members.push_back(c);  // añade al final
}

void Channel::remove(Client* c)
{
    if (!c)
        return;
    // find = busca desde .begin hasta .end el Client* c
    std::vector<Client*>::iterator it = std::find(_members.begin(), _members.end(), c);
    if (it != _members.end())
        _members.erase(it);   // borra al cliente si estaba
}