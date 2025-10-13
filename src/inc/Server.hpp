/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 16:26:01 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/13 14:30:33 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

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
# include <cstddef>
# include <sstream>
# include <cstdlib>
# include <cerrno>

# define SERVER_NAME	"IRCSERV"	// Server name
# define VERSION		"v.0.1"		// Server version
# define BACKLOG		20			// Max X persons in conexion queque
# define MAX_BITS_RD	512			// Max bits for reading
# define PING_INTERV	120			// x sec. Interval for PING
# define PING_TIMEOUT	60			// x sec. Timeout if client don't respond to PING

class Client;
class	Server
{
	private:
		int							_port;
		int							_server_fd;
		std::string					_pass;
		std::string					_pong;
		std::vector<pollfd>			_fds;
		std::map<int, Client*>		_clients;
		std::vector<std::string>	_commands;

		void	startServerVars(void);
		void	serverLoop(void);

		void	pingClient(int client_fd);
		void	ping_pong(int client_fd);

		/*-- CLIENT HANDLER --*/
		void	addNewClient();
		void	readClientData(int client_fd, std::string store);
		void	handleClientWrite(int client_fd);
		void	handleClientExit(size_t pos, int client_fd);
		void	sendMessageTo(int client_fd, std::string message);

		/*-- PARSER --*/
		void	processClientMsg(int client_fd);
		size_t	identifyCMD(std::string cmd, int client_fd);
		void	parseArgs(std::string input, int client_fd);
		void	executeCMD(int client_fd);

		/*	COMMANDS	*/
		std::string	sendWelcome(int client_fd);
		std::string	helpMe(size_t helpWith, int client_fd);
		std::string	pass(std::string password, int client_fd);
		std::string	nick(int client_fd);
		std::string	user(int client_fd);
		std::string	ping(int client_fd);
		std::string	pong(int client_fd);
		std::string	clear(void);
		std::string	serverStatus(void);

	public:
		Server();
		Server(int port, std::string pass);
		~Server();
		//** */

		void	startServer(int port, std::string pass);
};

#endif
