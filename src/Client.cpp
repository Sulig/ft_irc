/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 13:42:52 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/15 16:32:30 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "inc/Client.hpp"

# include <cerrno>
# include <sys/socket.h>
// # include <bits/socket.h> comentado para mi mac
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <cerrno>   // si usas errno / strerror
#include <cstring>  // si usas memset, strerror, etc.


/* Constructor & destructor */
void	Client::clientStartVars(void)
{
	_fd = 0;
	_is_logged = false;
	_is_registered = false;
	_is_welcomeSend = false;
	_pos = 0;
	_actualcmd.cmd_num = -1;
	_buffer = "";
	_sendbuffer = "";
	_nick = "";
	_user = "";
	_realname = "";
	_userModes = 0;
	_lastPingSent = 0;
	_lastPongSent = 0;
	_lastActivity = 0;
	_isPongSent = false;
	_isPongWaiting = false;
}
Client::Client()		{	clientStartVars();					}
Client::Client(int fd)	{	clientStartVars(); this->_fd = fd;	}
Client::~Client()	{}
/* ----- */

/*	GETTERS	*/
size_t	Client::getFD(void)	{	return (this->_fd);	}
size_t	Client::getPos(void)	{	return (this->_pos);	}
bool	Client::getIsLogged(void)	{	return (this->_is_logged);	}
bool	Client::getIsRegistered(void)	{	return (this->_is_registered);	}
bool	Client::getIsWelcomeSend(void)	{	return (this->_is_welcomeSend);	}
std::string	Client::getBuffer(void)	{	return (this->_buffer);	}
std::string	Client::getSendBuffer(void)	{	return (this->_sendbuffer);	}
std::string	Client::getNick(void)	{	return (this->_nick);	}
std::string	Client::getUser(void)	{	return (this->_user);	}
std::string	Client::getRealName(void)	{	return (this->_realname);	}

int		Client::getUserModes(void)	{	return (this->_userModes);	}

bool	Client::getIsPongSent(void)	{	return (_isPongSent);	}
bool	Client::getIsPongWaiting(void)	{	return (_isPongWaiting);	}
time_t	Client::getLastPingSent(void)	{	return (_lastPingSent);	}
time_t	Client::getLastPongSent(void)	{	return (_lastPongSent);	}
time_t	Client::getLastActivity(void)	{	return (_lastActivity);	}

std::vector<std::string>	Client::getActualCmdArgs(void)	{	return (this->_actualcmd.args);	}
t_command					Client::getActualCommand(void)	{	return (this->_actualcmd);	}
/* ----- */

/*	SETTERS	*/
void	Client::setPos(size_t pos)	{	this->_pos = pos;	}
void	Client::setIsLogged(bool logged)	{	this->_is_logged = logged;	}
void	Client::setIsRegistered(bool registerMe)	{	this->_is_registered = registerMe;	}
void	Client::setIsWelcomeSend(bool welcome)	{	this->_is_welcomeSend = welcome;	}
void	Client::setBuffer(std::string buffer)	{	this->_buffer = buffer;	}
void	Client::setSendBuffer(std::string _send)	{	this->_sendbuffer = _send;	}
void	Client::setNick(std::string nick)	{	this->_nick = nick;	}
void	Client::setUser(std::string user)	{	this->_user = user;	}
void	Client::setRealName(std::string name)	{	this->_realname = name;	}

void	Client::setUserModes(int modes)	{	_userModes |= modes;	}

void	Client::setIsPongSent(bool pong)	{	_isPongSent = pong;	}
void	Client::setIsPongWaiting(bool pong)	{	_isPongWaiting = pong;	}
void	Client::setLastPingSent(time_t time)	{	_lastPingSent = time;	}
void	Client::setLastPongSent(time_t time)	{	_lastPongSent = time;	}
void	Client::setLastActivity(time_t time)	{	_lastActivity = time;	}

void	Client::setActualCommand(t_command cmd)	{	this->_actualcmd = cmd;	}
/* ----- */

/*	/=/	*/
void	Client::appendToSendBuffer(std::string _send){	_sendbuffer += _send;	}

int	Client::sendPendingData(void)
{
	if (_sendbuffer.empty())
		return (0);

	int btss = send(_fd, _sendbuffer.c_str(), _sendbuffer.length(), MSG_DONTWAIT);
	if (btss > 0)
	{
		_sendbuffer.erase();
		return (btss);
	}
	// Not ready to send
	if (errno == EAGAIN || errno == EWOULDBLOCK)
		return (-1);
	else
		return (-2);
}
/* ----- */

// channel_name
void	Client::addChannel(const std::string& name)			{	channel_name.insert(name);				}
void	Client::removeChannel(const std::string& name)		{	channel_name.erase(name);				}
bool	Client::inChannel(const std::string& name) const	{	return (channel_name.count(name) != 0);	}
const	std::set<std::string>& Client::channels() const		{	return (channel_name);						}
