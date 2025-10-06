/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 13:42:52 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/06 14:14:07 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "inc/irc.hpp"

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
		//* Send welcome
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
	//todo -> set proper farewell }[vP]
	std::cout << CP << client_fd << " |Tv]// Come back soon!" << std::endl;
	close(client_fd);
	irc->clients.erase(client_fd);
	irc->fds.erase(irc->fds.begin() + pos);
	//todo -> also clear the `irc->clients[client_fd]` if some memory used.
}

void	readClientData(t_irc *irc, size_t pos, int client_fd)
{
	int btrd = 0;
	char tmp[512];

	size_t	found = irc->clients[client_fd].buffer.find_first_of("\r\n");
	do
	{
		btrd = recv(client_fd, tmp, sizeof(tmp) - 1, 0);
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
	//--
	std::cout << "Detected: " << irc->clients[client_fd].buffer << std::endl;
	std::string resp = "=[v)] Gochaa!!\n";
	send(client_fd, resp.c_str(), resp.size(), 0);
	//--
	//parsing
	//todo -> Remember to clean the buffer client when won't need it
	irc->clients[client_fd].buffer.clear();
}
