#ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include <map>
#include <string>

#include "channel.hpp"

struct t_irc;

class Channels
{
    private:
        std::map<std::string, Channel*> _byName;
    public:
        Channels();
        ~Channels();

        Channel* getOrCreate(const std::string& name); // crea si no existe
        Channel* find(const std::string& name) const;  // nullptr si no existe
        void     eraseIfEmpty(const std::string& name);
        void     removeClientEverywhere(const t_irc& irc, int fd); // para QUIT
};

#endif
