/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 17:42:53 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/08 20:24:16 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "inc/Server.hpp"
# include "inc/utils.hpp"
#include <sstream>

/* Constructor & destructor */
void	Server::startServerVars(void)
{
	_port = 0;
	_server_fd = 0;
	_commands.push_back("HELP");
	_commands.push_back("PASS");
	_commands.push_back("NICK");
	_commands.push_back("USER");
	_commands.push_back("PRIVMSG");
	_commands.push_back("QUIT");
	_commands.push_back("JOIN");
	_commands.push_back("PART");
	_commands.push_back("KICK");
	_commands.push_back("INVITE");
	_commands.push_back("TOPIC");
	_commands.push_back("MODE");
	_commands.push_back("PING");
	_commands.push_back("PONG");
	_commands.push_back("CLEAR");
	_commands.push_back("STATUS");
}
Server::Server(){	startServerVars();	}
Server::Server(int port, std::string pass){	startServerVars(); startServer(port, pass);	}
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
		_port = port;

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
			std::cerr << CRR << "Error: `poll` failed" << std::endl;
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
					processClientMsg(_fds[i].fd);
			}
			// There's something to send
			if (_fds[i].revents & POLLOUT)
				handleClientWrite(_fds[i].fd);
			// Client has disconnected
			if (_fds[i].revents & POLLHUP)
				handleClientExit(i, _fds[i].fd);
		}
	}
}

/*	Client Handler	*/
#pragma region CLIENT HANDLER
void	Server::addNewClient()
{
	try
	{
		struct sockaddr_in	client_addr;
		socklen_t	client_len = sizeof(client_addr);
		int client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &client_len);
		if (client_fd < 0)
			throw std::runtime_error("Client Accept failed");

		//	Set(F_SETFL) this socket/fd as No-block
		fcntl(client_fd, F_SETFL, O_NONBLOCK);

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

void	Server::sendWelcome(int client_fd)
{
	if (_clients.find(client_fd) == _clients.end())
		return ;
	std::string welcome = std::string(CWR);
	welcome += "* [###########################] *\r\n";
	welcome += "* \\          WELCOME          / *\r\n";
	welcome += "*  [#########################]  *\r\n";
	if (_clients[client_fd]->getIsLogged())
	{
		Client *client = _clients[client_fd];
		welcome += ":Welcome to the IRC Network, " + client->getNick() + "!\r\n";
		welcome += ":Your host is " + std::string(SERVER_NAME) + ", running version " + std::string(VERSION) + "\r\n";
		welcome += ":This server was created on: " + getCreationTime() + "\r\n";
	}
	else
	{
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

void	Server::handleClientExit(size_t pos, int client_fd)
{
	if (pos >= _fds.size())
	{
		std::cerr << CR << "Invalid position!" << DEF << std::endl;
		return ;
	}
	if (_fds[pos].fd != client_fd)
	{
		std::cerr << CP << "FD mismatch in handleClExit" << DEF << std::endl;
		bool	found = false;
		for (size_t i = 0; i < _fds.size(); i++) {
			if (_fds[i].fd == client_fd)
			{
				pos = i;
				found = true;
				break ;
			}
		}
		if (!found)
		{
			std::cerr << CR << "|" << client_fd << "| not found!" << DEF << std::endl;
			return ;
		}
	}
	//todo -> set proper farewell }[vP]
	std::cout << CP << client_fd << " |Tv]// Come back soon!" << DEF << std::endl;

	close(client_fd);

	if (_clients.find(client_fd) != _clients.end())
	{
		delete _clients[client_fd];
		_clients.erase(client_fd);
	}
	_fds.erase(_fds.begin() + pos);
	for (size_t i = 0; i < _fds.size(); i++)
		if (_clients.find(_fds[i].fd) != _clients.end())
			_clients[_fds[i].fd]->setPos(i);
}

void	Server::readClientData(int client_fd, std::string store)
{
	int		btrd = 1;
	char	buffer[513];

	btrd = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (btrd <= 0)
	{
		if (btrd == 0)
			std::cout << CY << "Client |" << client_fd << "| has disconnected." << DEF << std::endl;
		else
			std::cout << CR << "Error when reading from client |" << client_fd << "| :" << strerror(errno) << DEF << std::endl;
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			handleClientExit(_clients[client_fd]->getPos(), client_fd);
		return ;
	}
	buffer[btrd] = '\0';
	store += buffer;
	_clients[client_fd]->setBuffer(store);
}

void	Server::processClientMsg(int client_fd)
{
	if (_clients.find(client_fd) == _clients.end())
		return;

	readClientData(client_fd, _clients[client_fd]->getBuffer());

	if (_clients.find(client_fd) == _clients.end())
		return;

	std::string	buffer = _clients[client_fd]->getBuffer();
	if (buffer.size() > 512)
	{
		std::string msg = std::string(CR) + "Your message is too long!\r\n" + std::string(DEF);
		sendMessageTo(client_fd, msg);
		buffer.clear();
		return ;
	}
	_clients[client_fd]->setBuffer(buffer);
	buffer = normalizeCommand(buffer);
	identifyCMD(buffer, client_fd);
	parseCommand(_clients[client_fd]->getBuffer(), client_fd);
	executeCMD(client_fd);
	_clients[client_fd]->setBuffer("");
}
/* ----- */
#pragma endregion CLIENT HANDLER

#pragma region PARSER

size_t	Server::identifyCMD(std::string cmd, int client_fd)
{
	if (client_fd == 0)
	{
		for (size_t i = 0; i < _commands.size(); i++)
		{
			size_t	found = cmd.find(_commands[i], 0);
			if (found != std::string::npos)
				return (i);
		}
		return (-1);
	}
	_clients[client_fd]->setCommand(-1);
	for (size_t i = 0; i < _commands.size(); i++)
	{
		size_t	found = cmd.find(_commands[i], 0);
		if (found != std::string::npos)
		{
			_clients[client_fd]->setCommand(i);
			found += _commands[i].size();
			cmd = _clients[client_fd]->getBuffer().substr(found);
			_clients[client_fd]->setBuffer(cmd);
		}
	}
	return (_clients[client_fd]->getCommand());
}

void	Server::parseCommand(std::string input, int client_fd)
{
	std::vector<std::string>	args;
	std::istringstream iss(input);
	std::string token;

	while (iss >> token)
	{
		if (token[0] == ':')
		{
			std::string rest;
			std::getline(iss, rest);
			args.push_back(token.substr(1) + rest);
			break ;
		}
		args.push_back(token);
	}
	_clients[client_fd]->setAgrs(args);
	_clients[client_fd]->setBuffer("");
}

void	Server::executeCMD(int client_fd)
{
	if (_clients.find(client_fd) == _clients.end())
		return ;
	Client *client = _clients[client_fd];
	std::vector<std::string> tmp = client->getAgrs();
	std::string help = std::string(DEF);
	std::cout << client_fd << "| want to: " << itoa(_clients[client_fd]->getCommand()) << std::endl;
	if (!client->getIsLogged())
	{
		if (client->getCommand() != 0 && client->getCommand() != 1)
		{
			help += std::string(CR) + ":You're not logged yet!\r\n";
			help += std::string(CY) + ":Use the command PASS to login!" + std::string(DEF) + "\r\n";
		}
		else if (client->getCommand() == 1)
			help += pass(_clients[client_fd]->getAgrs()[0], client_fd);
		else
			help += helpMe(0, false);
		sendMessageTo(client_fd, help);
		return ;
	}
	switch (client->getCommand())
	{
		case 0:
			if (tmp.empty())
				sendMessageTo(client_fd, helpMe(0, true));
			else
			{
				tmp[0] = normalizeCommand(tmp[0]);
				sendMessageTo(client_fd, helpMe(identifyCMD(tmp[0], 0), true));
			}
		break ;

		case 1: pass(tmp[0], client_fd); break;

		case 14: sendMessageTo(client_fd, clear()); break;
		case 15: sendMessageTo(client_fd, serverStatus()); break;
		default:
			std::string help = std::string(CR) + ":This command don't exist or is not implemented yet." + std::string(DEF) + "\r\n";
			sendMessageTo(client_fd, help);
			break ;
	}
}

#pragma endregion PARSER

#pragma region COMMANDS

std::string	Server::helpMe(size_t helpWith, bool is_logged)
{
	std::string	help = std::string(CWR);
	if (!is_logged)
	{
		help += "*  [#########################]  *\r\n";
		help += "Thank you for using our server, to login follow this steps:\r\n";
		help += std::string(CC) + " :STEP 1 - Use PASS command:\r\n";
		help += helpMe(1, true);
		help += std::string(CC) + " :STEP 2 - Use NICK or USER command:\r\n";
		help += helpMe(2, true);
		help += std::string(CC) + " :STEP 3 - Have fun!\r\n";
		help += std::string(CP) + "If you need some help with a specific command, use HELP COMMAND" + std::string(DEF) + "\r\n";
	}
	else
	{
		switch (helpWith)
		{
			case 0:
			help += ":This are the supported commands:\r\n";
				for (size_t i = 0; i < _commands.size(); i++)
					help += "\t" + itoa(i) + "- |" + _commands[i] + "\r\n";
				help += "*  [#########################]  *\r\n";
				break;

			case 1:
			help += "Correct usage: PASS password\r\n";
			help += ":Loggin in server with the server password;\r\n";
			help += ":You can't use other commands until you're logged!\r\n";
			help += "\t* Advice: This will set your nickname with a number // Use NICK to change it\r\n";
				break;

			case 2:
			help += "Correct usage: NICK nikie\r\n";
			help += ":Set your custom nick;\r\n";
			help += "\t* Advice: Some characters are restricted!\r\n";
				break;

			case 3:
			help += "Correct usage: USER in-progress --not-implemented\r\n";
			help += ":Set your nick and real username;\r\n";
				break;

			case 14: help += "So many messages? -> Clear your terminal;\r\n"; break;
			case 15: help += "Print server status;"; break;

			default:
				help += std::string(CR) + ":The command that are you searching for don't exist!\r\n";
				help += std::string(DEF) + ":This are the supported commands:\r\n";
				for (size_t i = 0; i < _commands.size(); i++)
					help += "\t" + itoa(i) + "- |" + _commands[i] + "\r\n";
				help += "*  [#########################]  *\r\n";
				break;
		}
	}
	help += std::string(DEF);
	return (help);
}

std::string	Server::pass(std::string password, int client_fd)
{
	std::string	help = std::string(DEF);
	if (_clients[client_fd]->getIsLogged())
		return (std::string(CG) + "You're currently logged!\r\n" + std::string(CP) + "Maybe you wish to change your nick or send messages?\r\n" + std::string(DEF));
	if (password == _pass)
	{
		help += std::string(CGR) + "-- CORRECT PASSWORD --\r\n";
		help += std::string(CY) + " *Advice :Your nickname will be: " + itoa(_clients[client_fd]->getPos()) + "\r\n";
		help += "To change it, use the NICK command" + std::string(CC) + " :Have fun! :)" + std::string(DEF) + "\r\n";
		_clients[client_fd]->setIsLogged(true);
		_clients[client_fd]->setNick(itoa(_clients[client_fd]->getPos()));
	}
	else
		help += std::string(CRR) + "-- INCORRECT PASSWORD!! --" + std::string(DEF) + "\r\n";
	return (help);
}

std::string	Server::clear(void) {	return (std::string(CLEAN));	}
std::string	Server::serverStatus(void)
{
	std::string status = std::string(CWR);
	status += "\n* [###########################] *\r\n";
	status += "* \\          STATUS           / *\r\n";
	status += "*  [#########################]  *\r\n";
	status += ":" + std::string(SERVER_NAME) + " Status: |" + std::string(CG) + "Active" + std::string(DEF) + "|\r\n";
	status += ":Currently listening on port: " + std::string(CG) + itoa(_port) + std::string(DEF) + "\r\n";
	status += std::string(CY) + ":Clients connected so far: " + itoa(_fds.size() - 1) + std::string(DEF) + "\r\n";
	status += std::string(CC) + ":This server was created on: " + getCreationTime() + std::string(DEF) + "\r\n";
	// here maybe we can print the disponible channels
	std::cout << status;
	return (status);
}

#pragma endregion
