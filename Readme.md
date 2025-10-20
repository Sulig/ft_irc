Para hacer funcionar:

./ircserv 6667 secreto                              // por ejemplo, en T1 (terminal 1)

nc 127.0.0.1 6667                                   // en T2
PASS + <secreto>                                    // en T2
HELP                                                // te ensena la lista de cmds, puede pedir uno especifico
NICK + <nick>                                       // pones tu nickname
USER + <username> + <mode> + <unused> :<realname>   // USER samanta 0 (mode puede ser * tmb) * :Samanta Pascual
Si una terminal recive PING + num, se escribe en esa terminal PONG + num, si no se cierra.


## Connectar con Irssi:
/connect localhost 6667 word


NORMAS PARA CHANNELS:

Cada servidor puede tener muchos canales, y cada canal puede tener muchos usuarios.
Los canales siempre empiezan con #.
Los usuarios se unen con JOIN, salen con PART, hablan con PRIVMSG + #canal + msg ..., y los operadores pueden administrarlo (MODE, KICK, INVITE, etc.).
Mirar channel.hpp para ver lo que almacena un chat, como se ve


📌 Lo que pasa dentro del servidor (resumen):

JOIN    ➝ busca canal ➝ crea si no existe ➝ añade miembro ➝ broadcast JOIN      
PRIVMSG ➝ busca canal ➝ comprueba miembro ➝ broadcast mensaje                   ✓
PART    ➝ elimina miembro ➝ broadcast PART ➝ borra canal si vacío               ✓
MODE    ➝ cambia flags ➝ broadcast cambio                                       
INVITE  ➝ marca nick como invitado ➝ envía notificación                         ✓
KICK    ➝ elimina miembro ➝ broadcast KICK                                      ✓
QUIT    ➝ elimina usuario de todos los canales ➝ broadcast QUIT                 ✓

QUIT Y PART NECESARIOS ??
