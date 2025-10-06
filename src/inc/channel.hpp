#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>   // std::vector: lista dinámica (aqui son clientes)
#include <set>
#include <map>
#include <algorithm>
#include <cstddef>
#include <sys/socket.h>
#include <unistd.h>

#include "irc_structs.hpp" // otros de channel, que no se te olvide NO hacer circulares raras

struct t_client;   // viene de tu irc_structs.hpp
struct t_irc;      // idem

class Channel
{
    private:
        std::string           _name;          // "#general"
        std::string           _topic;         // puede estar vacío
        std::vector<int>      _members;       // fds de los clientes
        std::set<int>         _operators;     // fds con privilegio +o
        bool                  _mode_i;        // invite-only
        bool                  _mode_t;        // solo ops pueden cambiar topic
        bool                  _mode_k;        // canal protegido por clave
        bool                  _mode_l;        // límite de usuarios
        std::string           _key;           // clave si +k
        std::size_t           _limit;         // límite si +l
        std::set<std::string> _invitedNicks;  // nicks invitados (modo +i)

        Channel();                           // no permitido
        Channel(const Channel&);
        Channel& operator=(const Channel&);

    public:
        explicit Channel(const std::string& name);
        ~Channel();

        // Básicos
        const std::string& name()  const;
        const std::string& topic() const;
        std::size_t        size()  const;

        // Miembros
        bool  has(int fd) const;
        void  add(int fd);
        void  remove(int fd);
        bool  empty() const;

        // Operadores
        bool  isOp(int fd) const;
        void  giveOp(int fd);
        void  takeOp(int fd);

        // Topic
        void  setTopic(const std::string& t);
        bool  topicLocked() const; // +t

        // Modos
        void  setModeI(bool on);
        void  setModeT(bool on);
        void  setModeK(bool on, const std::string& key);
        void  unsetModeK();
        void  setModeL(bool on, std::size_t limit);
        void  unsetModeL();
        bool  modeI() const;
        bool  modeT() const;
        bool  modeK() const;
        bool  modeL() const;
        const std::string& key() const;
        std::size_t        limit() const;

        // Invitaciones (+i)
        void  inviteNick(const std::string& nick);
        bool  isInvited(const std::string& nick) const;
        void  clearInvite(const std::string& nick);

        // Broadcast
        void  broadcast(const t_irc& irc, const std::string& raw) const;
        void  broadcastExcept(const t_irc& irc, int exceptFd, const std::string& raw) const;

        // Serialización de modos (para RPL_CHANNELMODEIS)
        std::string modeString() const;     // p.ej. "+itkl 10 clave"
};


/*
con explicit:
Channel c = "#general";  // ❌ NO permitido (implícita)
Channel c("#general");   // ✅ Sí, construcción explícita
Sin explicit, C++ intentaría convertir const char* → std::string → Channel sin que tú lo pidas.
Forzamos que todo canal nazca con nombre válido (no hay constructor vacío → no hay Channel “sin nombre”).
Evitamos creaciones ambiguas o implícitas (gracias a explicit).
Eficiencia: no duplicamos std::string innecesariamente.
*/

/*
Channel representa un canal, obviamente, en irc, y los canales tienen JOIN (unirse),
part, TOPIC, MODE, INVITE, KICK Y QUIT. Los pondre en channel_cmds
*/

#endif
