#ifndef COMMANDS_CHANNEL_HPP
#define COMMANDS_CHANNEL_HPP

#include "channel.hpp"

class	Server;
class	Channels;

// Cada handler recibe el irc global, el fd del cliente y params (ya parseados)
void handleJOIN (Server& irc, Channels& chans, int fd, const std::vector<std::string>& params);
void handlePART (Server& irc, Channels& chans, int fd, const std::vector<std::string>& params);
void handleTOPIC(Server& irc, Channels& chans, int fd, const std::vector<std::string>& params);
void handleMODE (Server& irc, Channels& chans, int fd, const std::vector<std::string>& params);
void handleINVITE(Server& irc, Channels& chans, int fd, const std::vector<std::string>& params);
void handleKICK (Server& irc, Channels& chans, int fd, const std::vector<std::string>& params);
void handleQUIT (Server& irc, Channels& chans, int fd, const std::string& quitMsg);
void handlePRIVMSG(Server& irc, Channels& chans, int fd, const std::vector<std::string>& params);


/*
Reciben:
t_irc& irc (estado global: mapa de clientes, sus nicks, etc.)
Channels& chans (registro central de canales)
fd (socket del cliente que ejecuta el comando)
params (vector de strings ya tokenizados; JOIN puede traer listas separadas por comas)
(en QUIT, un quitMsg opcional)

Qué hacen (resumen):
Validan parámetros (si faltan → numérico de error).
Comprueban permisos/restricciones (modos, operador, pertenencia al canal).
Modifican el estado (añadir/quitar miembro, topic, modos…).
Notifican por broadcast lo que pasó (JOIN, PART, TOPIC, MODE…) y/o devuelven numéricos mínimos (332, 353, 366, etc.) suficientes para llevarse bien con irssi/hexchat.

Idea mental: cada handler es “si llega este comando → reglas → actualizo estado → notifico”.
*/

/*
IMPORTANT:

// En tu switch/if-else del parser:

if (cmd == "JOIN")   handleJOIN(irc, g_channels, fd, params);
else if (cmd == "PART")   handlePART(irc, g_channels, fd, params);
else if (cmd == "TOPIC")  handleTOPIC(irc, g_channels, fd, params);
else if (cmd == "MODE")   handleMODE(irc, g_channels, fd, params);
else if (cmd == "INVITE") handleINVITE(irc, g_channels, fd, params);
else if (cmd == "KICK")   handleKICK(irc, g_channels, fd, params);
else if (cmd == "QUIT")   handleQUIT(irc, g_channels, fd, trailingMsg);
*/


#endif
