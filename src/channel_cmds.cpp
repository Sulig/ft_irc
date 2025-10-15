#include "inc/channel_plural.hpp"
#include "inc/helpers.hpp"
#include "inc/Client.hpp"
#include "inc/Server.hpp"

// Small helpers
static std::string nickOrStar(Client* c) { return c ? c->getNick() : "*"; } // devuelve Nick del cliente si c != NULL
static std::string prefixOf(Client* c) { return ":" + nickOrStar(c) + "!" + nickOrStar(c) + "@localhost"; } // Crea la “cabecera” (prefijo) de un mensaje IRC, con el formato estándar ":nick!nick@localhost"

void handlePRIVMSG(Server& serv, Channels& chans, int fd, const std::vector<std::string>& params)
{
    Client* me = getClientFd(serv, fd);
    if (!me)
        return;

    if (params.size() < 2)
    {
        sendRawFd(fd, ":server 461 " + (me->getNick().empty() ? "*" : me->getNick()) + " PRIVMSG :Not enough parameters\r\n");
        return;
    }

    std::string target = params[0];
    std::string msg    = params[1]; // parser trae sin ':'

    // A canal
    if (isChannelName(target))
    {
        std::string chName = target[0] == '#' ? target : ("#" + target);
        Channel* ch = chans.find(chName);
        if (!ch)
        {
            sendRawFd(fd, ":server 403 " + me->getNick() + " " + chName + " :No such channel\r\n");
            return;
        }
        if (!ch->has(fd))
        {
            sendRawFd(fd, ":server 442 " + me->getNick() + " " + chName + " :You're not on that channel\r\n");
            return;
        }
        std::string raw = ":" + me->getNick() + "!" + me->getNick() + "@localhost PRIVMSG "
                        + chName + " :" + msg + "\r\n";
        ch->broadcastExcept(serv.getClients(), me, raw);
        return;
    }

    // A nick
    int dstFd = getFdByNick(serv, target);
    if (dstFd == -1)
    {
        sendRawFd(fd, ":server 401 " + me->getNick() + " " + target + " :No such nick/channel\r\n");
        return;
    }
    std::string raw = ":" + me->getNick() + "!" + me->getNick() + "@localhost PRIVMSG "
                    + target + " :" + msg + "\r\n";
    sendRawFd(dstFd, raw);
}




/* PART: PART <#chan> [<message>] */
void handlePART(Server& serv, Channels& chans, int fd, const std::vector<std::string>& params)
{
    Client* me = getClientFd(serv, fd);
    if (!me) return;

    if (params.empty()) {
        sendRawFd(fd, ":server 461 " + me->getNick() + " PART :Not enough parameters\r\n");
        return;
    }

    std::string chName = params[0];
    if (!isChannelName(chName)) chName = "#" + chName;

    Channel* ch = chans.find(chName);
    if (!ch || !ch->has(fd)) {
        sendRawFd(fd, ":server 442 " + me->getNick() + " " + chName + " :You're not on that channel\r\n");
        return;
    }

    std::string partMsg = (params.size() >= 2 ? params[1] : std::string());
    std::string raw = ":" + me->getNick() + "!" + me->getNick() + "@localhost PART "
                    + chName + (partMsg.empty() ? "" : " :" + partMsg) + "\r\n";

    ch->broadcast(serv.getClients(), raw);
    ch->remove(fd);
    chans.eraseIfEmpty(chName);
}






/* INVITE: INVITE <nick> <#chan> */
void handleINVITE(Server& serv, Channels& chans, int fd, const std::vector<std::string>& params)
{
    Client* me = getClientFd(serv, fd);
    if (!me)
        return;

    if (params.size() < 2)
    {
        sendRawFd(fd, ":server 461 " + me->getNick() + " INVITE :Not enough parameters\r\n");
        return;
    }

    const std::string nick = params[0];
    std::string chName     = params[1];

    if (!isChannelName(chName))
        chName = "#" + chName;

    Channel* ch = chans.find(chName);
    if (!ch)
    {
        sendRawFd(fd, ":server 403 " + me->getNick() + " " + chName + " :No such channel\r\n");
        return;
    }
    if (!ch->has(fd))
    {
        sendRawFd(fd, ":server 442 " + me->getNick() + " " + chName + " :You're not on that channel\r\n");
        return;
    }
    if (ch->modeI() && !ch->isOp(fd))
    {
        sendRawFd(fd, ":server 482 " + me->getNick() + " " + chName + " :You're not channel operator\r\n");
        return;
    }

    // localizar target
    int targetFd = getFdByNick(serv, nick);
    if (targetFd == -1)
    {
        sendRawFd(fd, ":server 401 " + me->getNick() + " " + nick + " :No such nick/channel\r\n");
        return;
    }
    if (ch->has(targetFd))
    {
        sendRawFd(fd, ":server 443 " + me->getNick() + " " + nick + " " + chName + " :is already on channel\r\n");
        return;
    }

    ch->inviteNick(nick);

    // 341 al que invita
    sendRawFd(fd, ":server 341 " + me->getNick() + " " + nick + " " + chName + "\r\n");

    // INVITE al objetivo
    std::string raw = ":" + me->getNick() + "!" + me->getNick() + "@localhost INVITE " + nick + " :" + chName + "\r\n";
    sendRawFd(targetFd, raw);
}





/* QUIT: QUIT [:message]  (we receive already-parsed quitMsg) */
void handleQUIT(Server& serv, Channels& chans, int fd, const std::string& quitMsg)
{
    Client* me = getClientFd(serv, fd);
    if (!me) return;

    // Si quieres hacer broadcast explícito del QUIT:
    // std::string raw = ":" + me->getNick() + "!" + me->getNick() + "@localhost QUIT"
    //                 + (quitMsg.empty() ? "" : " :" + quitMsg) + "\r\n";
    // (emitirlo en cada canal antes de remover)

    // Utilidad del gestor de canales que quite al user de todos los channels.
    // (Implementadla si no existe)
    chans.removeClientEverywhere(serv, fd);
}






/* KICK: KICK <#chan> <nick> [<comment>] */
void handleKICK(Server& serv, Channels& chans, int fd, const std::vector<std::string>& params)
{
    Client* me = getClientFd(serv, fd);
    if (!me)
        return;

    if (params.size() < 2)
    {
        sendRawFd(fd, ":server 461 " + me->getNick() + " KICK :Not enough parameters\r\n");
        return;
    }

    std::string chName = params[0];
    const std::string nick = params[1];
    std::string reason = (params.size() >= 3 ? params[2] : std::string());
    
    if (!isChannelName(chName))
        chName = "#" + chName;

    Channel* ch = chans.find(chName);
    if (!ch)
    {
        sendRawFd(fd, ":server 403 " + me->getNick() + " " + chName + " :No such channel\r\n");
        return;
    }
    if (!ch->has(fd))
    {
        sendRawFd(fd, ":server 442 " + me->getNick() + " " + chName + " :You're not on that channel\r\n");
        return;
    }
    if (!ch->isOp(fd))
    {
        sendRawFd(fd, ":server 482 " + me->getNick() + " " + chName + " :You're not channel operator\r\n");
        return;
    }

    int targetFd = getFdByNick(serv, nick);
    if (targetFd == -1 || !ch->has(targetFd))
    {
        sendRawFd(fd, ":server 441 " + me->getNick() + " " + nick + " " + chName + " :They aren't on that channel\r\n");
        return;
    }

    std::string raw = ":" + me->getNick() + "!" + me->getNick() + "@localhost KICK " + chName + " " + nick + (reason.empty()? "" : " :" + reason) + "\r\n";
    ch->broadcast(serv.getClients(), raw);
    ch->remove(targetFd);
    chans.eraseIfEmpty(chName);
}







/*
Handlers que implementan el comportamiento de cada comando ya parseado:

handleJOIN, handlePART, handleTOPIC, handleMODE, handleINVITE, handleKICK, handleQUIT.

Reciben:
Server *serv (estado global: mapa de clientes, sus nicks, etc.)
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
EXPLICACION DE CADA COMAND:

JOIN: JOIN #canal [clave]
ch = chans.getOrCreate(#canal).
Si +l y está lleno → 471 (channel full).
Si +k y no coincide clave → 475.
Si +i y no estaba invitado → 473.
Añade al miembro; si es el primero, dale +o.
broadcast(":nick!user@host JOIN #canal\r\n").
Si hay topic → 332 (topic).
Devuelve 353 (NAMES mínimo) y 366 (fin de NAMES).


PART: PART #canal [:msg]
Si canal no existe → 403.
Si no estás dentro → 442.
broadcast(":pref PART #canal :msg"), quita miembro, si vacío → eraseIfEmpty.


TOPIC: TOPIC #canal [:nuevoTopic]
Si sin parámetro → 461.
Si canal no existe → 403.
Si no estás en el canal → 442.
Sin :nuevoTopic ⇒ devuelve 331 (no topic) o 332 (topic actual).
Con :nuevoTopic ⇒ si +t y no eres op → 482. Si ok, setTopic() y broadcast.


MODE: MODE #canal [+|-][i t k l o] [args...]
Si sin canal → 461.
Si no existe → 403.
Solo consulta (solo MODE #canal) ⇒ 324 con modeString().
Cambios ⇒ debes ser operador (482 si no).
Soporta:
+i/-i, +t/-t, +k <key> / -k, +l <limit> / -l, +o <nick> / -o <nick>
Tras aplicar, broadcast con el estado actual.


INVITE: INVITE <nick> <#canal>
Validaciones: canal existe (403), estás dentro (442), si +i necesitas ser op (482).
Marca al nick como invitado (permite JOIN si +i).
Respuestas:
Al que invita: 341 (RPL_INVITING).
Al invitado: NOTICE de invitación.


KICK: KICK <#canal> <nick> [:comentario]
Debes ser op (482).
El nick debe estar en el canal (441 si no).
broadcast(":pref KICK #canal nick :coment") y remove.


QUIT: QUIT [:msg]
Saca al cliente de todos los canales (removeClientEverywhere).
(Opcional: antes podrías broadcast QUIT a cada canal donde esté.)
Cierra socket y limpia del irc.clients (esto último normalmente ya lo hace tu loop principal).
*/