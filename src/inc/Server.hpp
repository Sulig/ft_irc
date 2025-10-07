/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 16:26:01 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/07 14:21:28 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "colors.hpp"


# include <fcntl.h>
# include <poll.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <unistd.h>

# include <algorithm>
# include <map>
# include <vector>
# include <iostream>
# include <cstring>
# include <stdexcept>
# include <cerrno>

# define SERVER_NAME	"IRCSERV"
# define VERSION		"v.0.1"
# define BACKLOG		20	// Max X persons in conexion queque

/*	COMMANDS	*/
# define UNKNOW	-1
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

# define PING	'\\'
# define PONG	'/'

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
		void	handleClientWrite(int client_fd);
		void	handleClientExit(size_t pos, int client_fd);
		void	sendWelcome(int client_fd);
		void	sendMessageTo(int client_fd, std::string message);

		/*-- PARSER --*/
		int		identifyCMD(std::string cmd);
		void	setClientCommand(int client_fd);

	public:
		Server();
		Server(int port, std::string pass);
		~Server();
		//** */

		void	startServer(int port, std::string pass);
};

#endif
