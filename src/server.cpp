/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 17:42:53 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/02 19:41:52 by sadoming         ###   ########.fr       */
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
	server_poll.revents = 0;
	irc.fds.push_back(server_poll);

	while (true)
	{
		int _poll = poll(&irc.fds[0], irc.fds.size(), -1);
		if (_poll < 0)
		{
			std::cerr << CR << "Error: `poll` failed" << std::endl;
			break ;
		}
		for (size_t i = 0; i < irc.fds.size(); i++)
		{
			// There's some available data
			if (irc.fds[i].revents & POLLIN)
			{
				if (irc.fds[i].fd == server_fd)
					handleNewClient(server_fd, &irc);
				else
					handleClientData(&irc, i, irc.fds[i].fd, pass);
			}
			if (irc.fds[i].revents & POLLHUP)
				handleClientExit(&irc, i, irc.fds[i].fd);
		}
		std::cout << CY << "Clients connected so far: " << irc.fds.size() - 1 << DEF << std::endl;
	}
}

void	handleNewClient(int server_fd, t_irc *irc)
{
	try
	{
		struct sockaddr_in	client_addr;
		socklen_t	client_len =sizeof(client_addr);
		int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
		if (client_fd < 0)
			throw std::runtime_error("Client Accept failed");

		pollfd	new_client;
		new_client.fd = client_fd;
		new_client.events = POLLIN;
		new_client.revents = 0;
		irc->fds.push_back(new_client);

		t_client	client;
		memset(&client, 0, sizeof(t_client));
		client.fd = new_client.fd;
		irc->clients[new_client.fd] = client;

		std::cout << CCR << "New Client connected, with FD = " << new_client.fd << DEF << std::endl;
		return ;
	}
	catch (const std::exception& e)
	{
		std::cerr << CR << "Error: " << e.what() << DEF << std::endl;
		return ;
	}
}

void	handleClientExit(t_irc *irc, size_t pos, int client_fd)
{
	std::cout << CP << client_fd << " |Tv]// Come back soon!" << std::endl;
	close(client_fd);
	irc->clients.erase(client_fd);
	irc->fds.erase(irc->fds.begin() + pos);
	//todo -> also clear the `irc->clients[client_fd]` if some memory used.
}

void	handleClientData(t_irc *irc, size_t pos, int client_fd, std::string pass)
{
	(void)pass;
	int btrd = 0;
	char tmp[512];

	std::cout << "The [" << pos << "] is tring to send something." << std::endl;
	std::cout << "Client FD = " << client_fd << std::endl;
	size_t	found = irc->clients[client_fd].buffer.find_first_of("\r\n");
	do
	{
		btrd = recv(client_fd, tmp, sizeof(tmp) - 1, 0);
		std::cout << "btrd recieved: " << btrd << std::endl;
		if (btrd == 0)
		{
			handleClientExit(irc, pos, client_fd);
			return ;
		}
		else if (btrd > 0)
		{
			tmp[btrd] = '\0';
			irc->clients[client_fd].buffer += tmp;
			if (irc->clients[client_fd].buffer.empty())
				return ;
		}
		else
		{
			std::cerr << CR << "Error detected when reciving client message" << DEF << std::endl;
			return ;
		}

		found = irc->clients[client_fd].buffer.find_first_of("\r\n");
	} while (btrd > 0 && found == std::string::npos);
	std::cout << "Detected: " << irc->clients[client_fd].buffer << std::endl;
	std::string resp = "=[v)] Gochaa!!\n";
	send(client_fd, resp.c_str(), resp.size(), 0);
	//todo -> Remember to clean the buffer client when won't need it
	irc->clients[client_fd].buffer.clear();
}
