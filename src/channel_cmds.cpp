#include "inc/channel_plural.hpp"
#include "inc/helpers.hpp"
#include "inc/Client.hpp"
#include "inc/Server.hpp"
#include <iostream>

// Helper preventivo
static std::string normChan(std::string s)
{
    // quita espacios al principio y al final
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t'))
        s.erase(s.begin());
    while (!s.empty() && (s.back()  == ' ' || s.back()  == '\t'))
        s.pop_back();
    // quita \r y \n por si se colaron
    for (size_t i = 0; i < s.size();)
    {
        if (s[i] == '\r' || s[i] == '\n')
            s.erase(i, 1);
        else
            ++i;
    }
    if (s.empty())
        return s;
    if (s[0] != '#')
        s.insert(s.begin(), '#');
    return s;
}

// PRIVMSG: PRIVMSG <#channel> <:msg> || sin los : solo coge hasta el primer espacio
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
        std::string chName = normChan(target);
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





void handlePART(Server& serv, Channels& chans, int fd, const std::vector<std::string>& params)
{
    (void)serv;
    (void)chans;
    (void)fd;
    (void)params;

    std::cerr << "Part not implemented" << "\n";
}

/* PART: PART <#chan> [<message>] 
void handlePART(Server& serv, Channels& chans, int fd, const std::vector<std::string>& params)
{
    Client* me = getClientFd(serv, fd);
    if (!me) return;

    if (params.empty())
    {
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
*/





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






void handleQUIT(Server& serv, Channels& chans, int fd, const std::string& quitMsg)
{
    (void)serv;
    (void)chans;
    (void)fd;
    (void)quitMsg;

    std::cerr << "Quit not implemented" << "\n";
}

/* QUIT: QUIT [:message] 
void handleQUIT(Server& serv, Channels& chans, int fd, const std::string& quitMsg)
{
    Client* me = getClientFd(serv, fd);
    if (!me)
        return;

    // Mensaje que se enviará a todos los canales donde estaba el usuario
    std::string raw = ":" + me->getNick() + "!" + me->getNick() + "@localhost QUIT"
                    + (quitMsg.empty() ? " :Client Quit" : " :" + quitMsg) + "\r\n";

    // Notifica a todos los canales que el usuario ha salido
    chans.removeClientEverywhere(serv, fd);

    // Envía el mensaje de QUIT al propio usuario (opcional)
    sendRawFd(fd, raw);

    // Cierra el socket del cliente, pero NO el servidor
    close(fd);

    // Elimina el cliente del mapa de clientes y del vector de poll
    std::map<int, Client*> clients = serv.getClients();
    std::map<int, Client*>::iterator it = clients.find(fd);
    if (it != clients.end())
    {
        delete it->second;
        clients.erase(it);
    }

    // En clase Server un método tipo removeClient(fd) ?
    // directamente:
    // serv.removeClient(fd);
}
*/






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




/* JOIN: JOIN <#chan> <"key"> */
void handleJOIN(Server& serv, Channels& chans, int fd, const std::vector<std::string>& params)
{
    Client* me = getClientFd(serv, fd);
    if (!me)
        return;

    if (params.empty())
    {
        sendRawFd(fd, ":server 461 " + me->getNick() + " JOIN :Not enough parameters\r\n");
        return;
    }

    // nombre normalizado
    std::string chName = normChan(params[0]);
    std::string key    = (params.size() >= 2 ? params[1] : ""); // mode +k

    Channel* ch = chans.find(chName);
    // DEBUG
    std::cerr << "[DBG] find channel  raw='" << params[0]
          << "'  norm='" << chName
          << "'  user='" << me->getNick()
          << "'  ptr=" << (const void*)ch;
    if (!ch)
    {
        ch = chans.getOrCreate(chName);
        ch->giveOp(fd);
    }
    else
    {
        if (ch->modeI() && !ch->isInvited(me->getNick()))
        {
            sendRawFd(fd, ":server 473 " + me->getNick() + " " + chName + " :Cannot join channel (+i)\r\n");
            return;
        }
        if (ch->modeK() && ch->key() != key)
        {
            sendRawFd(fd, ":server 475 " + me->getNick() + " " + chName + " :Cannot join channel (+k)\r\n");
            return;
        }
        if (ch->modeL() && ch->size() >= ch->limit())
        {
            sendRawFd(fd, ":server 471 " + me->getNick() + " " + chName + " :Cannot join channel (+l)\r\n");
            return;
        }
    }

    ch->add(fd);

    std::string raw = ":" + me->getNick() + "!" + me->getNick() + "@localhost JOIN " + chName + "\r\n";
    ch->broadcast(serv.getClients(), raw);

    if (!ch->topic().empty())
        sendRawFd(fd, ":server 332 " + me->getNick() + " " + chName + " :" + ch->topic() + "\r\n");
    else
        sendRawFd(fd, ":server 331 " + me->getNick() + " " + chName + " :No topic is set\r\n");

    std::string names = ch->namesList(serv.getClients());
    sendRawFd(fd, ":server 353 " + me->getNick() + " = " + chName + " :" + names + "\r\n");
    sendRawFd(fd, ":server 366 " + me->getNick() + " " + chName + " :End of /NAMES list\r\n");
}





/* TOPIC: TOPIC <#chan> [<topic>] */
void handleTOPIC(Server& serv, Channels& chans, int fd, const std::vector<std::string>& params)
{
    Client* me = getClientFd(serv, fd);
    if (!me)
        return;

    if (params.empty())
    {
        sendRawFd(fd, ":server 461 " + me->getNick() + " TOPIC :Not enough parameters\r\n");
        return;
    }

    std::string chName = params[0];
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

    // Ver topic
    if (params.size() == 1)
    {
        if (ch->topic().empty())
            sendRawFd(fd, ":server 331 " + me->getNick() + " " + chName + " :No topic is set\r\n");
        else
            sendRawFd(fd, ":server 332 " + me->getNick() + " " + chName + " :" + ch->topic() + "\r\n");
        return;
    }

    // Cambiar topic
    if (ch->modeT() && !ch->isOp(fd))
    {
        sendRawFd(fd, ":server 482 " + me->getNick() + " " + chName + " :You're not channel operator\r\n");
        return;
    }

    std::string newTopic = params[1];
    ch->setTopic(newTopic);

    std::string raw = ":" + me->getNick() + "!" + me->getNick() + "@localhost TOPIC " + chName + " :" + newTopic + "\r\n";
    ch->broadcast(serv.getClients(), raw);
}





/* MODE: MODE <#chan> <modes> params... " " */
void handleMODE(Server& serv, Channels& chans, int fd, const std::vector<std::string>& params)
{
    Client* me = getClientFd(serv, fd);
    if (!me)
        return;

    if (params.empty())
    {
        sendRawFd(fd, ":server 461 " + me->getNick() + " MODE :Not enough parameters\r\n");
        return;
    }

    std::string chName = params[0];
    if (!isChannelName(chName))
        chName = "#" + chName;

    Channel* ch = chans.find(chName);
    if (!ch)
    {
        sendRawFd(fd, ":server 403 " + me->getNick() + " " + chName + " :No such channel\r\n");
        return;
    }

    if (params.size() == 1)
    {
        sendRawFd(fd, ":server 324 " + me->getNick() + " " + chName + " " + ch->modeString() + "\r\n");
        return;
    }

    if (!ch->isOp(fd))
    {
        sendRawFd(fd, ":server 482 " + me->getNick() + " " + chName + " :You're not channel operator\r\n");
        return;
    }

    std::string modes = params[1];
    bool adding = true;
    size_t paramIndex = 2;

    for (size_t i = 0; i < modes.size(); ++i)
    {
        char c = modes[i];
        if (c == '+') adding = true;
        else if (c == '-') adding = false;
        else if (c == 'i') ch->setModeI(adding);
        else if (c == 't') ch->setModeT(adding);
        else if (c == 'k')
        {
            if (adding && paramIndex < params.size())
                ch->setModeK(true, params[paramIndex++]);
            else
                ch->unsetModeK();
        }
        else if (c == 'l')
        {
            if (adding && paramIndex < params.size())
                ch->setModeL(true, atoi(params[paramIndex++].c_str()));
            else
                ch->unsetModeL();
        }
    }

    std::string raw = ":" + me->getNick() + "!" + me->getNick() + "@localhost MODE " + chName + " " + modes + "\r\n";
    ch->broadcast(serv.getClients(), raw);
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