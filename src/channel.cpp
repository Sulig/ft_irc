#include "inc/channel.hpp"
#include "inc/Client.hpp"
#include "inc/helpers.hpp"
#include <iostream>

// constructor
Channel::Channel(const std::string& name) : _name(name), _topic(""),
                                            _mode_i(false), _mode_t(false),
                                            _mode_k(false), _mode_l(false),
                                            _key(""), _limit(0) {}

Channel::~Channel() {}

Channel& Channel::operator=(const Channel& other)
{
    if (this != &other)
    {
        // Copia superficial (shallow)
        // Si hay que "duplicar" channels (no suele tener sentido), cambiar esto.
        // No se usa nunca esto, esta en privado
        this->_name         = other._name;
        this->_topic        = other._topic;
        this->_members      = other._members; // vector<Client*>
        this->_operators    = other._operators; // set<int> o set de lo que uses
        this->_mode_i       = other._mode_i;
        this->_mode_t       = other._mode_t;
        this->_mode_k       = other._mode_k;
        this->_mode_l       = other._mode_l;
        this->_key          = other._key;
        this->_limit        = other._limit;
        this->_invitedNicks = other._invitedNicks;  // set<std::string>
    }
    return *this;
}

const std::string& Channel::name() const 
{
    return _name;
}

const std::string& Channel::topic() const
{
    return _topic;
}

std::size_t Channel::size() const 
{
    return _members.size();
}

bool Channel::empty() const
{
    return _members.empty();
}

bool Channel::has(int fd) const
{
    return std::find(_members.begin(), _members.end(), fd) != _members.end();
}

void Channel::add(int fd)
{
    if (!has(fd)) _members.push_back(fd);
    std::cerr << "[DBG] JOIN to " << _name << " this=" << (const void*)this
          << " add fd=" << fd << " size=" << _members.size() << "\n";
}

void Channel::remove(int fd)
{
    std::vector<int>::iterator it = std::find(_members.begin(), _members.end(), fd);
    if (it != _members.end())
        _members.erase(it);
    _operators.erase(fd);
}

bool Channel::isOp(int fd) const
{
    return _operators.count(fd) != 0;
}
void Channel::giveOp(int fd)
{
    _operators.insert(fd);
}

void Channel::takeOp(int fd)
{
    _operators.erase(fd);
}

void Channel::setTopic(const std::string& t)
{
    _topic = t;
}

bool Channel::topicLocked() const
{
    return _mode_t;
}

void Channel::setModeI(bool on)
{
    _mode_i = on;
}

void Channel::setModeT(bool on)
{
    _mode_t = on;
}

void Channel::setModeK(bool on, const std::string& key)
{
    _mode_k = on; _key = on ? key : "";
}

void Channel::unsetModeK()
{
    _mode_k = false; _key.clear();
}

void Channel::setModeL(bool on, std::size_t limit)
{
    _mode_l = on; _limit = on ? limit : 0;
}

void Channel::unsetModeL()
{
    _mode_l = false; _limit = 0;
}

bool Channel::modeI() const
{
    return _mode_i;
}

bool Channel::modeT() const
{
    return _mode_t;
}

bool Channel::modeK() const
{
    return _mode_k;
}

bool Channel::modeL() const
{
    return _mode_l;
}

const std::string& Channel::key() const
{
    return _key;
}

std::size_t Channel::limit() const
{
    return _limit;
}

void Channel::inviteNick(const std::string& nick)
{
    _invitedNicks.insert(nick);
}

bool Channel::isInvited(const std::string& nick) const
{
    return _invitedNicks.count(nick)!=0;
}

void Channel::clearInvite(const std::string& nick)
{
    _invitedNicks.erase(nick);
}

void Channel::broadcast(const std::map<int, Client*>& clients, const std::string& raw) const
{
    // 'raw' debe acabar en \r\n
    for (size_t i = 0; i < _members.size(); ++i)
    {
        int fd = _members[i];
        if (clients.find(fd) == clients.end())
            continue;
        sendRawFd(fd, raw);
    }
}

void Channel::broadcastExcept(const std::map<int, Client*>& clients, Client* except, const std::string& raw) const
{
    for (size_t i = 0; i < _members.size(); ++i)
    {
        int fd = _members[i];
        std::map<int, Client*>::const_iterator it = clients.find(fd);
        if (it == clients.end())
            continue;
        if (it->second == except)
            continue; // no eco al emisor
        sendRawFd(fd, raw);
    }
}


std::string Channel::modeString() const
{
    std::string s = "+";
    std::string args;
    if (_mode_i) s += "i";
    if (_mode_t) s += "t";
    if (_mode_k) s += "k";
    if (_mode_l) s += "l";
    // argumentos en orden k l
    if (_mode_k && !_key.empty()) args += " " + _key;
    if (_mode_l && _limit)
    {
        std::ostringstream ss;
        ss << _limit;
        args += " " + ss.str();
    }
    if (s == "+") s.clear(); // sin modos activos
    return s + args;
}

std::string Channel::namesList(const std::map<int, Client*>& clients) const
{
    std::string result;
    for (std::vector<int>::const_iterator i = _members.begin(); i != _members.end(); ++i)
    {
        std::map<int, Client*>::const_iterator c = clients.find(*i);
        if (c != clients.end())
        {
            if (!_operators.empty() && _operators.count(*i))
                result += "@" + c->second->getNick();
            else
                result += c->second->getNick();
            if (i + 1 != _members.end())
                result += " ";
        }
    }
    return result;
}


/*
Estado:
name (ej. #cafe)
topic (string)
members (vector de FDs conectados)
operators (set de FDs con +o)
modos: +i (invite-only), +t (topic solo ops), +k <key>, +l <limit>
invitedNicks (a quién se ha invitado si +i)

Operaciones:
add(fd), remove(fd), has(fd) → gestionar miembros
giveOp(fd), takeOp(fd) → dar/quitar operador
setTopic(), topicLocked() → gestionar tópico y bloqueo +t
setModeI/T/K/L y unset → activar/desactivar modos
inviteNick(nick), isInvited(nick), clearInvite(nick) → gestión de invitaciones
broadcast(raw) / broadcastExcept(fd, raw) → enviar un mensaje a todos (o a todos menos uno)
modeString() → devuelve algo como +itkl clave 10 para informar modos

Idea mental: Channel es una cajita con listas de gente dentro y banderas de cómo se comporta.
*/