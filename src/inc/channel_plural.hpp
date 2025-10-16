#ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include "channel.hpp"

class Server;

class Channels
{
    private:
        std::map<std::string, Channel*> _byName;
    public:
        Channels();
        ~Channels();

        Channel*    getOrCreate(const std::string& name); // crea si no existe
        Channel*    find(const std::string& name) const;  // nullptr si no existe
        void        eraseIfEmpty(const std::string& name); // si no hay nadie en el canal...
        void        removeClientEverywhere(Server serv, int fd); // para QUIT
};

#endif
