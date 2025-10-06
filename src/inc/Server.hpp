/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 16:26:01 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/06 17:50:38 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "colors.hpp"

# include <poll.h>

# include <map>
# include <vector>
# include <iostream>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <unistd.h>

# include <cstring>
# include <stdexcept>

# define BACKLOG	20	// Max X persons in conexion queque

/*	COMMANDS	*/
# define HELP	0
# define PASS	1
# define NICK	2
# define PRIVMSG	3

# define QUIT	4
# define USER	'U'
# define JOIN	'J'
# define PART	'P'
# define KICK	'K'
# define INVITE	'I'
# define TOPIC	'T'
# define MODE	'M'

# define PING	'\p'
# define PONG	'\t'

class	Server
{
	private:
		int						_server_fd;
		std::string				_pass;
		std::vector<pollfd>		_fds;
		std::map<int, Client*>	_clients;

		void	serverLoop(void);

		/*-- CLIENT HANDLER --*/
		void	addNewClient();
		void	readClientData(size_t pos, int client_fd);
		void	handleClientExit(size_t pos, int client_fd);

	public:
		Server();
		Server(int port, std::string pass);
		~Server();
		//** */

		void	startServer(int port, std::string pass);
};

#endif
