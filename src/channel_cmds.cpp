#include "channel_cmds.hpp"











/*
Handlers que implementan el comportamiento de cada comando ya parseado:

handleJOIN, handlePART, handleTOPIC, handleMODE, handleINVITE, handleKICK, handleQUIT.

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