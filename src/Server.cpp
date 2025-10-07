/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 17:42:53 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/07 14:32:27 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "inc/Server.hpp"
# include "inc/utils.hpp"

/* Constructor & destructor */
Server::Server(){	_server_fd = 0;	}
Server::Server(int port, std::string pass){	startServer(port, pass);	}
Server::~Server(){}
/* ----- */

/*	/=/	*/
void	Server::startServer(int port, std::string pass)
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
		_server_fd = socket(PF_INET, SOCK_STREAM, 0);
		if (_server_fd < 0)
			throw std::runtime_error("Cannot create socket");

		//* 3- Configure options
		int opt	= 1;
		/* Reuse the address next time when server start (SO_REUSEADDR)
			- the socket fd
			- socket level
			- resuse address macro
			- opt 1 -> Activate
		*/
		if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("Cannot defime socket options");

		//* 4- Bind
		// Link the socket to an IP (address + port) //* if port is in use will return -1
		if (bind(_server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
			throw std::runtime_error("Cannot bind socket");

		//* 5- Listen
		// Listening calls (queque clients to 20)
		if (listen(_server_fd, BACKLOG) < 0)
			throw std::runtime_error("Cannot listen on soket");

		std::cout << CGR << "IRC Server started on port: " << port << DEF << std::endl;

		/*	-- Conf	--	*/
		pollfd	server_poll;
		server_poll.fd = _server_fd;
		server_poll.events = POLLIN;
		server_poll.revents = 0;
		this->_pass = pass;
		_fds.push_back(server_poll);

		// Start server
		serverLoop();

		//* Last -- Close the server (close the file-descriptor)
		close(_server_fd);
	}
	catch (const std::exception& e)
	{
		std::cerr << CRR << "Server error: " << e.what() << DEF << std::endl;
		return ;
	}
}

void	Server::serverLoop(void)
{
	while (true)
	{
		int _poll = poll(&_fds[0], _fds.size(), -1);
		if (_poll < 0)
		{
			std::cerr << CR << "Error: `poll` failed" << std::endl;
			break ;
		}
		for (size_t i = 0; i < _fds.size(); i++)
		{
			// There's some available data
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _server_fd)
					addNewClient();
				else
					readClientData(i, _fds[i].fd);
			}
			// There's something to send
			if (_fds[i].revents & POLLOUT)
				handleClientWrite(_fds[i].fd);
			// Client has disconnected
			if (_fds[i].revents & POLLHUP)
				handleClientExit(i, _fds[i].fd);
		}
		std::cout << CY << "Clients connected so far: " << _fds.size() - 1 << DEF << std::endl;
	}
}

/*	Client Handler	*/
void	Server::addNewClient()
{
	try
	{
		struct sockaddr_in	client_addr;
		socklen_t	client_len =sizeof(client_addr);
		int client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &client_len);
		if (client_fd < 0)
			throw std::runtime_error("Client Accept failed");

		pollfd	new_client;
		new_client.fd = client_fd;
		new_client.events = POLLIN;
		new_client.revents = 0;
		_fds.push_back(new_client);

		Client	*client = new Client(new_client.fd);
		_clients[new_client.fd] = client;
		_clients[new_client.fd]->setPos(_fds.size() - 1);

		std::cout << CCR << "New Client connected, with FD = " << new_client.fd << DEF << std::endl;
		sendWelcome(new_client.fd);
		return ;
	}
	catch (const std::exception& e)
	{
		std::cerr << CR << "Error: " << e.what() << DEF << std::endl;
		return ;
	}
}

void	Server::sendWelcome(int client_fd)
{
	if (_clients.find(client_fd) == _clients.end())
		return ;
	std::string welcome = "";
	if (_clients[client_fd]->getIsLogged())
	{
		Client *client = _clients[client_fd];
		welcome += ":Welcome to the IRC Network, " + client->getNick() + "!\r\n";
		welcome += ":Your host is " + std::string(SERVER_NAME) + ", running version " + std::string(VERSION) + "\r\n";
		welcome += ":This server was created " + getCreationTime() + "\r\n";
	}
	else
	{
		welcome += std::string(CWR) + "* [###########################] *\r\n";
		welcome += "* \\          WELCOME          / *\r\n";
		welcome += "*  [#########################]  *\r\n";
		welcome += std::string(CY) + "\n> You're not registered yet!\r\n";
		welcome += "> You must register first using the PASS & NICK commands.\r\n";
		welcome += std::string(CC) + "* \\ Send HELP or /help for more info / *" + std::string(DEF) + "\r\n";
	}
	//Actualize the events of this client to send welcome
	_fds[_clients[client_fd]->getPos()].events = POLLIN | POLLOUT;
	_clients[client_fd]->setSendBuffer(welcome);
}

void	Server::sendMessageTo(int client_fd, std::string message)
{
	if (_clients.find(client_fd) == _clients.end())
		return ;
	//Actualize the events of this client to send the message
	_fds[_clients[client_fd]->getPos()].events = POLLIN | POLLOUT;
	_clients[client_fd]->setSendBuffer(message);
}

void	Server::handleClientWrite(int client_fd)
{
	if (_clients.find(client_fd) == _clients.end())
		return ;
	int result = _clients[client_fd]->sendPendingData();
	if (result == -2)
		handleClientExit(_clients[client_fd]->getPos(), client_fd);
	else if (result >= 0 && _clients[client_fd]->getSendBuffer().empty())
		_fds[_clients[client_fd]->getPos()].events = POLLIN;
}

void	Server::handleClientExit(size_t pos, int client_fd)
{
	//todo -> set proper farewell }[vP]
	std::cout << CP << client_fd << " |Tv]// Come back soon!" << std::endl;

	if (_clients.find(client_fd) != _clients.end())
	{
		delete _clients[client_fd];
		_clients.erase(client_fd);
	}
	close(client_fd);
	_fds.erase(_fds.begin() + pos);
}

void	Server::readClientData(size_t pos, int client_fd)
{
	int btrd = 0;
	char tmp[512];

	std::string	buffer = _clients[client_fd]->getBuffer();
	buffer.clear();
	size_t	found = buffer.find_first_of("\r\n");
	do
	{
		btrd = recv(client_fd, tmp, sizeof(tmp) - 1, 0);
		if (btrd == 0)
		{
			handleClientExit(pos, client_fd);
			return ;
		}
		else if (btrd > 0)
		{
			tmp[btrd] = '\0';
			buffer += tmp;
			if (buffer.empty())
				return ;
		}
		else
		{
			std::cerr << CR << "Error detected when reciving client message" << DEF << std::endl;
			return ;
		}
		found = buffer.find_first_of("\r\n");
	} while (btrd > 0 && found == std::string::npos);
	//--
	std::string resp = "=[v)] Gochaa!\n";
	sendMessageTo(client_fd, resp);
	//--
	buffer = normalizeCommand(buffer);
	_clients[client_fd]->setBuffer(buffer);
	_clients[client_fd]->setCommand(identifyCMD(buffer));
	std::cout << client_fd << "| Want to: " << buffer << ", nm." << identifyCMD(buffer) << std::endl;
}
/* ----- */

int	Server::identifyCMD(std::string cmd)
{
	if (cmd == "PASS")
		return PASS;
	if (cmd == "NICK")
		return NICK;
	if (cmd == "USER")
		return USER;
	if (cmd == "PRIVMSG")
		return PRIVMSG;
	if (cmd == "QUIT")
		return QUIT;
	if (cmd == "JOIN")
		return JOIN;
	if (cmd == "PART")
		return PART;
	if (cmd == "KICK")
		return KICK;
	if (cmd == "INVITE")
		return INVITE;
	if (cmd == "TOPIC")
		return TOPIC;
	if (cmd == "MODE")
		return MODE;
	if (cmd == "PING")
		return PING;
	if (cmd == "PONG")
		return PONG;
	if (cmd == "HELP")
		return HELP;

	return (UNKNOW);
}
