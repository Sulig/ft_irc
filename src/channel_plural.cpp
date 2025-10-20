# include "inc/Server.hpp"
# include "inc/Client.hpp"
# include "inc/channel.hpp"
# include "inc/channel_plural.hpp"
# include "inc/channel_cmds.hpp"

static std::string normName(std::string s)
{
    // trim inicio/fin (espacios y tabs)
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t'))
    s.erase(s.begin());
    while (!s.empty() && (s.back()  == ' ' || s.back()  == '\t'))
    s.pop_back();

    // elimina CR/LF rezagados
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());

    if (s.empty())
        return s;
    if (s[0] != '#')
        s.insert(s.begin(), '#');
    return s;
}

Channels::Channels() {}

Channels::~Channels()
{
    for (std::map<std::string, Channel*>::iterator it = _byName.begin(); it != _byName.end(); ++it)
        delete it->second;
    _byName.clear();
}

Channel* Channels::getOrCreate(const std::string& name)
{
    std::string key = normName(name);
    std::map<std::string, Channel*>::iterator it = _byName.find(key);
    // it = #general || 

    if (it != _byName.end())
        return it->second; // Cada entrada del mapa es un par: it->first → la clave (el nombre del canal) : it->second → el valor (el Channel* guardado)

    Channel* c = new Channel(key);
    _byName[key] = c;
    return c;
}

Channel* Channels::find(const std::string& name) const
{
    std::string key = normName(name);
    std::map<std::string, Channel*>::const_iterator it = _byName.find(key);
    return it==_byName.end()? NULL : it->second;
}

void Channels::eraseIfEmpty(const std::string& name)
{
    std::string key = normName(name);
    std::map<std::string, Channel*>::iterator it = _byName.find(key);
    if (it != _byName.end() && it->second->empty())
    {
        delete it->second;
        _byName.erase(it);
    }
}

void Channels::removeClientEverywhere(Server serv, int fd)
{
    (void)serv;
    for (std::map<std::string, Channel*>::iterator it = _byName.begin(); it != _byName.end(); ++it)
        it->second->remove(fd);
    // No borramos mapas aquí; que el controlador de QUIT llame a eraseIfEmpty por cada canal si quiere.
}


/*
Es un mapa: map<string, Channel*> para localizar canales por nombre.

Funciones:
getOrCreate(name) → si no existe, lo crea (# automático si falta).
find(name) → devuelve puntero o NULL.
eraseIfEmpty(name) → borra canal vacío (y delete).
removeClientEverywhere(irc, fd) → saca un FD de todos los canales (para QUIT).

Idea mental: Channels es la guía telefónica de canales.
*/