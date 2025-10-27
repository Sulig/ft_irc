/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 17:42:53 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/27 20:12:35 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "inc/Server.hpp"
# include "inc/Client.hpp"
# include "inc/channel.hpp"
# include "inc/channel_plural.hpp"
# include "inc/channel_cmds.hpp"

/* Constructor & destructor */
void	Server::startServerVars(void)
{
	_port = 0;
	_server_fd = 0;
	_pass = "";
	_pong = "";
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
	_commands.push_back("STAT");
	_commands.push_back("CAP");
}
Server::Server(){	startServerVars();	}
Server::Server(int port, std::string pass){	startServerVars(); startServer(port, pass);	}
Server::~Server()
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}
	_clients.clear();
	if (_server_fd > 0)
		close(_server_fd);
}
/* ----- */

/*	GETTERS	*/
Client	*Server::getClient(int fd)
{
	if (_clients.find(fd) == _clients.end())
		return (NULL);
	else
		return (_clients[fd]);
}
std::map<int, Client*>	Server::getClients(void)	{	return (_clients);	}
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
			ping_pong(_fds[i].fd);
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

void	Server::sendMessageTo(int client_fd, std::string message)
{
	if (_clients.find(client_fd) == _clients.end())
		return ;
	//Actualize the events of this client to send the message
	_fds[_clients[client_fd]->getPos()].events = POLLIN | POLLOUT;
	if (_clients[client_fd]->getSendBuffer().empty())
		_clients[client_fd]->setSendBuffer(message);
	else
		_clients[client_fd]->appendToSendBuffer(message);
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
	std::cout << CP << client_fd << "|> Come back soon!" << DEF << std::endl;

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
	char	buffer[MAX_BITS_RD + 1];
	size_t	found = store.find_first_of("\r\n");

	while (btrd > 0 && found == std::string::npos)
	{
		btrd = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (btrd == 0)
		{
			std::cout << CY << "Client |" << client_fd << "| has disconnected." << DEF << std::endl;
			handleClientExit(_clients[client_fd]->getPos(), client_fd);
			return ;
		}
		else if (btrd > 0)
		{
			buffer[btrd] = '\0';
			store += buffer;
			if (store.empty())
				return ;
		}
		else
		{
			std::cout << CR << "Error when reading from client |" << client_fd << "| :" << strerror(errno) << DEF << std::endl;
			if (!(errno != EAGAIN && errno != EWOULDBLOCK))
				break ;
		}
		found = store.find_first_of("\r\n");
	}
	if (_clients.find(client_fd) == _clients.end())
		return;
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
	if (buffer.size() > MAX_BITS_RD)
	{
		std::string msg = std::string(CR) + "Your message is too long!\r\n" + std::string(DEF);
		sendMessageTo(client_fd, msg);
		buffer.clear();
		return ;
	}
	else if (buffer.find_first_of("\r\n") == std::string::npos)
		return ;

	while (buffer[0] == '/' || buffer[0] == '\\' || buffer[0] == ' ')
		buffer = buffer.substr(1);

	_clients[client_fd]->setBuffer(buffer);
	while (buffer.find_first_of("\r\n") != std::string::npos)
	{
		std::string	line = buffer.substr(0, buffer.find_first_of("\r\n"));
		buffer = buffer.substr(buffer.find("\n") + 1);

		t_command	cmd;
		cmd.cmd_num = identifyCMD(normalizeCommand(line));
		if (!(cmd.cmd_num >= _commands.size()))
			line = line.substr(_commands[cmd.cmd_num].size());
		cmd.args = parseArgs(line);
		_clients[client_fd]->setActualCommand(cmd);
		executeCMD(client_fd, cmd);
		//* Clean after using this command.
		cmd.cmd_num = -1;
		cmd.args.clear();
		if (_clients.find(client_fd) == _clients.end())
			return ;
		_clients[client_fd]->setActualCommand(cmd);
	}
	_clients[client_fd]->setBuffer("");
}
/* ----- */
#pragma endregion CLIENT HANDLER

#pragma region PARSER

size_t	Server::identifyCMD(std::string cmd)
{
	for (size_t i = 0; i < _commands.size(); i++)
	{
		size_t	found = cmd.find(_commands[i], 0);
		if (found != std::string::npos)
			return (i);
	}
	return (-1);
}

std::vector<std::string>	Server::parseArgs(std::string input)
{
	std::vector<std::string>	args;
	size_t	pos = 0, last = 0;

	if (input.find("\n") != std::string::npos)
		input = input.substr(0, input.find("\n"));
	while (pos < input.size())
	{
		pos = input.find_first_not_of(' ', pos);
		if (pos < input.size())
		{
			if (input[pos] == ':')
			{
				args.push_back(input.substr(pos + 1));
				break ;
			}
			last = input.find_first_of(' ', pos);
			if (last > input.size())
				last = input.size();
			args.push_back(input.substr(pos, last - pos));
			pos = last + 1;
		}
	}
	return (args);
}

void	Server::executeCMD(int client_fd, t_command cmd)
{
	if (_clients.find(client_fd) == _clients.end())
		return ;
	if (cmd.cmd_num >= _commands.size() && cmd.args.size() == 0)
		return ;

	Client *client = _clients[client_fd];
	std::string help = std::string(DEF);

	/**/ // Only for debug ->
	std::cout << client_fd << "| want to: " << cmd.cmd_num << "| with args: ";
	if (cmd.args.size() == 0)
		std::cout << "| no args |" << std::endl;
	else
		for (size_t i = 0; i < cmd.args.size(); i++)
			std::cout << "|" << i << "| " << cmd.args[0] << std::endl;
	/**/

	if (!client->getIsLogged())
	{
		if (cmd.cmd_num != 0 && cmd.cmd_num != 1 && cmd.cmd_num != 16)
		{
			help += std::string(CR) + ":You're not logged yet!\r\n";
			help += std::string(CY) + ":Use the command PASS to login!" + std::string(DEF) + "\r\n";
			sendMessageTo(client_fd, help);
		}
		else if (cmd.cmd_num == 16)
			sendMessageTo(client_fd, cap(client_fd));
		else if (cmd.cmd_num == 0)
			sendMessageTo(client_fd, helpMe(0, client_fd));
		else if (cmd.cmd_num == 1)
			sendMessageTo(client_fd, pass(client_fd));
		_clients[client_fd]->setLastActivity(time(NULL));
		return ;
	}
	if (!client->getIsRegistered())
	{
		switch (cmd.cmd_num)
		{
			case 0: sendMessageTo(client_fd, helpMe(0, client_fd)); break ;
			case 1: sendMessageTo(client_fd, pass(client_fd)); break;
			case 2: sendMessageTo(client_fd, nick(client_fd)); break;
			case 3: sendMessageTo(client_fd, user(client_fd)); break;
			case 14: sendMessageTo(client_fd, clear()); break;
			case 16: sendMessageTo(client_fd, cap(client_fd)); break;
		default:
			std::string help = std::string(CR) + ":You don't have the access to this command 'till you register OR This command don't exist." + std::string(DEF) + "\r\n";
			sendMessageTo(client_fd, help);
			break ;
		}
		_clients[client_fd]->setLastActivity(time(NULL));
		return ;
	}

	// he tocado el switch
	switch (cmd.cmd_num)
	{
		case 0:
			if (cmd.args.empty())
				sendMessageTo(client_fd, helpMe(0, 1));
			else
			{
				cmd.args[0] = normalizeCommand(cmd.args[0]);
				sendMessageTo(client_fd, helpMe(identifyCMD(cmd.args[0]), 1));
			}
		break ;
		case 1: sendMessageTo(client_fd, pass(client_fd)); break;
		case 2: sendMessageTo(client_fd, nick(client_fd)); break;
		case 3: sendMessageTo(client_fd, user(client_fd)); break;
		case 4: handlePRIVMSG(*this, _channels, client_fd, cmd.args); break;

		// Nuevos handlers de canales
		case 5: handleQUIT(*this, _channels, client_fd, ""); quit(client_fd); break;
		case 6: handleJOIN(*this, _channels, client_fd, cmd.args); break;
		case 7: handlePART(*this, _channels, client_fd, cmd.args); break;
		case 8: handleKICK(*this, _channels, client_fd, cmd.args); break;
		case 9: handleINVITE(*this, _channels, client_fd, cmd.args); break;
		case 10: handleTOPIC(*this, _channels, client_fd, cmd.args); break;
		case 11: handleMODE(*this, _channels, client_fd, cmd.args); break;

		case 12: sendMessageTo(client_fd, ping(client_fd)); break;
		case 13: sendMessageTo(client_fd, pong(client_fd)); break;
		case 14: sendMessageTo(client_fd, clear()); break;
		case 15: sendMessageTo(client_fd, serverStatus()); break;
		case 16: sendMessageTo(client_fd, cap(client_fd)); break;

		default:
		{
			std::string help = std::string(CR) + ":This command don't exist or is not implemented yet." + std::string(DEF) + "\r\n";
			sendMessageTo(client_fd, help);
			break ;
		}
	}
	if (_clients.find(client_fd) == _clients.end())
		return ;
	_clients[client_fd]->setLastActivity(time(NULL));
}

#pragma endregion PARSER

#pragma region COMMANDS

void	Server::sendWelcome(int client_fd)
{
	if (_clients.find(client_fd) == _clients.end())
		return ;
	Client	*client = _clients[client_fd];
	std::string welcome = "";
	if (client->getIsRegistered() && !client->getIsWelcomeSend())
	{
		welcome = ":" + std::string(SERVER_NAME) + " 001 " + client->getNick() + " :Welcome to the IRC Network, " + client->getNick() + "! ";
		welcome += client->getUser() + "@localhost\r\n";
		sendMessageTo(client_fd, welcome);
		welcome = ":" + std::string(SERVER_NAME) + " 002 " + client->getNick() + " :Your host is " + std::string(SERVER_NAME) + ", running version " + std::string(VERSION) + "\r\n";
		sendMessageTo(client_fd, welcome);
		welcome = ":" + std::string(SERVER_NAME) + " 003 " + client->getNick() + " :This server was created on: " + getCreationTime() + "\r\n";
		sendMessageTo(client_fd, welcome);
		welcome = ":" + std::string(SERVER_NAME) + " 004 " + client->getNick() + " localhost 1.0 -- --\r\n";
		sendMessageTo(client_fd, welcome);
		welcome.clear();
		_clients[client_fd]->setIsWelcomeSend(true);
		std::cout << CG << "Client " << client_fd << " (" << client->getNick() << ") fully registered and welcomed." << DEF << std::endl;
	}
	else if (!_clients[client_fd]->getIsIrrsi())
	{
		welcome += std::string(CY) + "> You're not registered yet!\r\n";
		welcome += "> You must register first using the PASS, NICK && USER commands.\r\n";
		welcome += std::string(CC) + "* \\ Send HELP or /help for more info / *" + std::string(DEF) + "\r\n";
		sendMessageTo(client_fd, welcome);
	}
}

std::string	Server::helpMe(size_t helpWith, int client_fd)
{
	std::string	help = std::string(CWR);
	if (client_fd == 0 || client_fd == 1)
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
			help += "Correct usage: USER username mode    *   :Real Name\r\n";
			help += "Other usage:   USER username host server :Real Name\r\n";
			help += ":Set your username and Real Name;\r\n";
			help += ":'mode' params accepted:\r\n";
			help += "\t* Advice: This must be used only once, so think your Real Name!\r\n";
				break;

			case 4:
			help += "Correct usage: PRIVMSG sent_to :Message\r\n";
			help += ":Send a private message to a destinatary;\r\n";
			help += "\t* You can also send a message to a channel using '#' (Ej: PRIVMSG #group :Hello!)\r\n";
				break;

			case 12:
			help += "Correct usage: PING token\r\n";
			help += ":Send \"PING\" to the server (checks if SERVER is still connected);\r\n";
				break;
			case 13:
			help += "Correct usage: PONG token\r\n";
			help += ":Send \"PONG\" to the server (checks if YOU are still connected);\r\n";
				break;
			case 14: help += "So many messages? -> Clear your terminal;\r\n"; break;
			case 15: help += "Print server status;"; break;
			case 16:
			help += "Correct usage: CAP LS\r\n";
			help += ":List server capabilities (none yet..);\r\n";
				break;
			default:
				help += std::string(CR) + ":The command that are you searching for don't exist!\r\n";
				help += std::string(DEF) + ":This are the supported commands:\r\n";
				for (size_t i = 0; i < _commands.size(); i++)
					help += "\t" + itoa(i) + "- |" + _commands[i] + "\r\n";
				help += "*  [#########################]  *\r\n";
				break;
		}
	}
	else
	{
		if (!(_clients[client_fd]->getIsLogged()))
		{
			help += "*  [#########################]  *\r\n";
			help += "Thank you for using our server, to login follow this steps:\r\n";
			help += std::string(CC) + " :STEP 1 - Use PASS command:\r\n";
			help += helpMe(1, true);
			help += std::string(CC) + " :STEP 2 - Use NICK command:\r\n";
			help += helpMe(2, true);
			help += std::string(CC) + " :STEP 3 - Use USER command:\r\n";
			help += helpMe(3, true);
			help += std::string(CC) + " :STEP 4 - Have fun!\r\n";
			help += std::string(CP) + "If you need some help with a specific command, use HELP COMMAND" + std::string(DEF) + "\r\n";
		}
		else if (!(_clients[client_fd]->getIsRegistered()))
		{
			help += "*  [#########################]  *\r\n";
			help += std::string(CG) + ":You're currently logged;\r\n" + std::string(DEF);
			help += std::string(CP) + ":But you're not registered yet!\r\n" + std::string(DEF);
			help += ":To register, you need to:\r\n";
			if (_clients[client_fd]->getNick().empty())
				help += "* Use NICK command!\r\n" + helpMe(2, true);
			if (_clients[client_fd]->getUser().empty())
				help += "* Use USER command!\r\n" + helpMe(3, true);
		}
	}
	help += std::string(DEF);
	return (help);
}

std::string	Server::cap(int client_fd)
{
	std::vector<std::string>	args = _clients[client_fd]->getActualCmdArgs();

	if (args.empty())
		return ("CAP :No subcommand\r\n");
	if (args[0].find("LS") != std::string::npos)
	{
		_clients[client_fd]->setIsIrrsi(true);
		return ("CAP * LS :\r\n");
	}
	else if (args[0].find("END") != std::string::npos)
	{
		if (_clients[client_fd]->getIsRegistered())
			sendWelcome(client_fd);
		return ("");
	}
	else
		return ("CAP :Invalid subcommand\r\n");
}

std::string	Server::pass(int client_fd)
{
	std::string	help = std::string(DEF);
	std::vector<std::string>	args = _clients[client_fd]->getActualCmdArgs();

	if (args.empty())
		return (std::string(CR) + "Password not given!" + std::string(DEF) + "\r\n");

	if (_clients[client_fd]->getIsLogged())
		return (std::string(CG) + "You're currently logged!\r\n" + std::string(CP) + "Maybe you wish to change your nick or send messages?\r\n" + std::string(DEF));

	if (args[0] == _pass)
	{
		help += std::string(CGR) + "-- CORRECT PASSWORD --" + std::string(DEF) + "\r\n";
		_clients[client_fd]->setIsLogged(true);
	}
	else
		help += std::string(CRR) + "-- INCORRECT PASSWORD!! --" + std::string(DEF) + "\r\n";
	return (help);
}

std::string	Server::nick(int client_fd)
{
	std::string	help = std::string(DEF);
	std::vector<std::string>	args = _clients[client_fd]->getActualCmdArgs();

	if (args.empty())
		return (std::string(CR) + ":Nickname not given!" + std::string(DEF) + "\r\n");

	std::string	nick = args[0];
	if (nick.size() > NICK_MAX_CHARS)
		return (std::string(CR) + ":Nickname is too long!" + std::string(DEF) + "\r\n");

	if (!(identifyCMD(nick) >= _commands.size()))
		return (std::string(CR) + ":Nickname can't be one of the commands!" + std::string(DEF) + "\r\n");

	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		if (nick == it->second->getNick())
			return (std::string(CY) + ":Nickname is already in use!" + std::string(DEF) + "\r\n");

	if (std::string(NICK_MUST_NOT_STARTWITH).find_first_of(nick[0]) != std::string::npos)
		return (std::string(CR) + ":Nickname must not start with: " + std::string(NICK_MUST_NOT_STARTWITH) + std::string(DEF) + "\r\n");

	if (nick.find_first_of(NICK_MUST_NOT_CONTAIN) != std::string::npos)
		return (std::string(CR) + ":Nickname must not contain: " + std::string(NICK_MUST_NOT_CONTAIN) + std::string(DEF) + "\r\n");

	if (isAllPrintable(nick) == false)
		return (std::string(CR) + ":Nickname must contain only printable chars!" + std::string(DEF) + "\r\n");

	_clients[client_fd]->setNick(nick);
	help += std::string(CG) + ":Nickname changed successfully to:" + nick + std::string(DEF) + "\r\n";
	sendMessageTo(client_fd, help);
	help.clear();
	if (!(_clients[client_fd]->getUser().empty()))
	{
		_clients[client_fd]->setIsRegistered(true);
		if (!_clients[client_fd]->getIsWelcomeSend() && !_clients[client_fd]->getIsIrrsi())
			sendWelcome(client_fd);
	}
	else
		help = std::string(CP) + " *Don't forget to add a username to! (use USER)" + std::string(DEF) + "\r\n";
	return (help);
}

std::string	Server::user(int client_fd)
{
	std::string	help = std::string(DEF);
	std::vector<std::string>	args = _clients[client_fd]->getActualCmdArgs();

	if (_clients[client_fd]->getIsRegistered())
		return (std::string(CPR) + ":You're already registered!" + std::string(DEF) + "\r\n");

	if (!(_clients[client_fd]->getUser().empty()))
		return (std::string(CR) + ":You can't modify your username!" + std::string(DEF) + "\r\n");

	if (args.empty())
		return (std::string(CR) + ":No args given!" + std::string(DEF) + "\r\n");
	if (args.size() < 4)
		return (std::string(CR) + ":Need more parameters!\r\n" + std::string(CY) + " - Maybe you forget the '*'?" + std::string(DEF) + "\r\n");

	//** USERNAME CHECKER */
	std::string	username = args[0];

	if (username.find_first_of(USER_MUST_NOT_CONTAIN) != std::string::npos)
		return (std::string(CR) + ":Username must not contain: " + std::string(NICK_MUST_NOT_CONTAIN) + std::string(DEF) + "\r\n");

	if (isAllPrintable(username) == false)
		return (std::string(CR) + ":Username must contain only printable chars!" + std::string(DEF) + "\r\n");

	_clients[client_fd]->setUser(username);
	help += std::string(CG) + ":Username changed successfully to:" + username + std::string(DEF) + "\r\n";
	//*** */

	std::string	realName = args[3];
	if (isAllPrintable(realName) == false)
		return (std::string(CR) + ":Real Name must contain only printable chars! (accents and umlauts not supported)" + std::string(DEF) + "\r\n");

	_clients[client_fd]->setRealName(realName);
	help += std::string(CG) + ":Real Name changed successfully to:" + realName + std::string(DEF) + "\r\n";
	sendMessageTo(client_fd, help);
	help.clear();
	if (!(_clients[client_fd]->getNick().empty()))
	{
		_clients[client_fd]->setIsRegistered(true);
		if (!_clients[client_fd]->getIsWelcomeSend() && !_clients[client_fd]->getIsIrrsi())
			sendWelcome(client_fd);
	}
	else
		help = std::string(CP) + " *Don't forget to add a nikname to! (use NICK)" + std::string(DEF) + "\r\n";
	return (help);
}

/*
std::string	Server::privmsg(int client_fd)
{
	std::string	help = std::string(DEF);
	std::vector<std::string>	args = _clients[client_fd]->getActualCmdArgs();

	if (args.size() < 2)
		return (std::string(CR) + ":Need more args!" + std::string(DEF) + "\r\n");

	if (args[0][0] != '#')
	{
		for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		{
			if (args[0] == it->second->getNick())
			{
				sendMessageTo(it->first, args[1]);
				return ("");
			}
		}
		return (std::string(CR) + ":Nickname not found!" + std::string(DEF) + "\r\n");
	}
	else if (args[0].find_first_of("*?") != std::string::npos)
	{
		// permitir que solo los operadores usen wilcards..
		return (std::string(CP) + ":Feature not added yet!" + std::string(DEF) + "\r\n");
	}
	else
	{
		//seach if channel exist in server
		//get the <vector> || <map> of clients connected in this channel if encountered.
		//send the msg for this clients
		return (std::string(CP) + ":The channel feature is not implemented yet.." + std::string(DEF) + "\r\n"); // This is temporal.
	}
	return (help);
}
*/

void	Server::quit(int client_fd)
{
	if (_clients.find(client_fd) == _clients.end())
		return ;
	std::string	help = std::string(DEF);
	std::vector<std::string>	args = _clients[client_fd]->getActualCmdArgs();
	std::string goodbye = "";

	if (!args.size())
		goodbye = "Disconecting with QUIT";
	else
		goodbye = args[0];
	std::cout << CP << "\t" << _clients[client_fd]->getNick() << "|> " << goodbye << std::endl;
	// -- send the message to all chanels that client has joined to
	//----------
	handleClientExit(_clients[client_fd]->getPos(), client_fd);
}

#pragma region PING - PONG
std::string	Server::ping(int client_fd)
{
	std::string	help = std::string(DEF);
	std::vector<std::string>	args = _clients[client_fd]->getActualCmdArgs();

	if (args.empty())
		return (std::string(CR) + ":Args not given!" + std::string(DEF) + "\r\n");

	help += std::string(CWR) + "9\\ * PING - \"" + args[0] + "\" - Recieved from: |" + itoa(client_fd) + "|";
	help += ", alias \"" + _clients[client_fd]->getNick() + "\"\r\n" + std::string(DEF);

	std::cout << help;
	help = std::string(CWR) + "- \"" + args[0] + "\" - */P PONG!\r\n" + std::string(DEF);
	return (help);
}

void	Server::pingClient(int client_fd)
{
	if (_clients.find(client_fd) == _clients.end())
		return ;
	if (_clients[client_fd]->getIsPongWaiting())
		return ;

	_pong = itoa(rand());
	std::string help = std::string(CWR) + "9\\ * PING - \"" + _pong + "\" -\r\n" + std::string(DEF);
	std::cout << "~ * ~ Sended a PING to: " << client_fd << "with: " << _pong << " as token" << std::endl;
	_clients[client_fd]->setIsPongSent(false);
	_clients[client_fd]->setIsPongWaiting(true);
	_clients[client_fd]->setLastPingSent(time(NULL));
	sendMessageTo(client_fd, help);
}

void	Server::ping_pong(int client_fd)
{
	time_t	now = time(NULL);

	if (_clients.find(client_fd) == _clients.end())
		return ;
	if (!_clients[client_fd]->getIsRegistered())
		return ;
	if (_clients[client_fd]->getIsPongWaiting())
	{
		time_t	last_ping = _clients[client_fd]->getLastPingSent();
		if (now - last_ping > PING_TIMEOUT)
		{
			std::string	help = std::string(CYR) + itoa(client_fd) + " ** TIMEOUT!! - Desconecting..\r\n" + std::string(CYR);
			handleClientExit(_clients[client_fd]->getPos(), client_fd);
		}
		return ;
	}
	time_t	last_activity = _clients[client_fd]->getLastActivity();
	if (now - last_activity > PING_INTERV)
		pingClient(client_fd);
}

std::string	Server::pong(int client_fd)
{
	std::string	help = std::string(DEF);
	std::vector<std::string>	args = _clients[client_fd]->getActualCmdArgs();

	if (args.empty())
		return (std::string(CR) + ":Args not given!" + std::string(DEF) + "\r\n");
	if (args[0] != _pong)
		return (std::string(CR) + ":Invalid PONG token!" + std::string(DEF) + "\r\n");
	if (_clients[client_fd]->getIsPongSent())
		return (std::string(CY) + ":Alredy sent a PONG. Wait for next PING..." + std::string(DEF) + "\r\n");

	_clients[client_fd]->setIsPongWaiting(false);
	_clients[client_fd]->setIsPongSent(true);
	_clients[client_fd]->setLastPongSent(time(NULL));

	help += std::string(CWR) + "|" + itoa(client_fd) + "|";
	help += ", alias \"" + _clients[client_fd]->getNick() + "\"";
	help += "- \"" + args[0] + "\" - */P PONG!\r\n" + std::string(DEF);
	std::cout << help;
	help.clear();
	return (help);
}

#pragma endregion PING - PONG

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
