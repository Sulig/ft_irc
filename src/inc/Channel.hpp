#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>   // std::vector: lista dinámica (aqui son clientes)
#include <cstddef>
#include <algorithm>

class Client;

class Channel
{
    private:
        std::string          _name;     // nombre del canal: "#general"
        std::string          _topic;    // topic del canal (puede estar vacío)
        std::vector<Client*> _members;  // lista de punteros a clientes que están dentro

        // const: prohibimos copia por accidente, garantizas que cada canal es unico y te ahorras bugs con punteros a Client
        Channel(const Channel&);
        Channel& operator=(const Channel&);

    public:
        explicit Channel(const std::string& name); // crear canal con un nombre
        ~Channel();                                 // destructor (vacío, pero declarado)

        // Getters (const al final es firma de: “esta función NO modifica el objeto”)
        const std::string& name()  const; // const std::string&, no modificar objeto, evita la copia
        const std::string& topic() const;
        std::size_t        getSize()  const; // cuántos miembros hay
        bool               has(Client* c) const; // ¿está este cliente dentro?

        // Setters (modifican el estado del canal)
        void setTopic(const std::string& t); // _topic = t
        void add(Client* c);      // mete a un cliente, si no está
        void remove(Client* c);   // saca a un cliente, si está

        // Utilidad: enviar un mensaje ya formateado a todos los miembros
        void broadcast(const std::string& raw) const; // requiere que Client tenga algo tipo sendRaw()
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

#endif
