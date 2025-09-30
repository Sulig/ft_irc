/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 17:42:53 by sadoming          #+#    #+#             */
/*   Updated: 2025/09/30 20:31:37 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "inc/irc.hpp"
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>

void	startServer(int port, std::string pass)
{
	try
	{
		//* 1- Configure the `sock` struct
		struct sockaddr_in	address;
		memset(&address, 0, sizeof(sockaddr_in));
		address.sin_family = AF_INET;				// Ipv4
		address.sin_addr.s_addr = INADDR_ANY;		// Listen all interfaces
		address.sin_port  = htons(port);	// User port (this)

		//* 2- Create socket
		//	PF_INET (Ipv4 (Same as AF_INET, but more conventional)) -- SOCK_STREAM (TCP) -- 0 (Deffault)
		int	server_fd = socket(PF_INET, SOCK_STREAM, 0);
		if (server_fd < 0)
			throw std::runtime_error("Cannot create socket");

		//* 3- Configure options
		int opt	= 1;
		/* Reuse the address next time when server start (SO_REUSEADDR)
			- the socket fd
			- socket level
			- resuse address macro
			- opt 1 -> Activate
		*/
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("Cannot defime socket options");

		//* 4- Bind
		// Link the socket to an IP (address + port) //* if port is in use will return -1
		if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
			throw std::runtime_error("Cannot bind socket");

		//* 5- Listen
		// Listening calls (queque clients to 20)
		if (listen(server_fd, BACKLOG) < 0)
			throw std::runtime_error("Cannot listen on soket");

		std::cout << CGR << "IRC Server started on port: " << port << DEF << std::endl;
		serverLoop(server_fd, pass);

		//* Last -- Close the server (close the file-descriptor)
		close(server_fd);
		//* Maybe here do clean?
	}
	catch (const std::exception& e)
	{
		std::cerr << CRR << "Server error: " << e.what() << DEF << std::endl;
		return ;
	}
}

void	serverLoop(int server_fd, std::string pass)
{
	t_irc	irc;
	pollfd	server_poll;
	server_poll.fd = server_fd;
	server_poll.events = POLLIN;
	irc.fds.push_back(server_poll);

	while (true)
	{
		int _poll = poll(&irc.fds[0], irc.fds.size(), -1);
		if (_poll < 0)
		{
			std::cerr << CR << "Error: `poll` failed" << std::endl;
			break ;
		}

		//* Look each fd --> for
		//for ()
	}
}
