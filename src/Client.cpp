/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 13:42:52 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/13 14:24:33 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "inc/Client.hpp"
# include "inc/Server.hpp"

/* Constructor & destructor */
Client::Client()	{	_fd = 0; _is_logged = false;	}
Client::Client(int fd)	{	this->_fd = fd; _is_logged = false;	}
Client::~Client()	{}
/* ----- */

/*	GETTERS	*/
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

int		Client::getCommand(void)	{	return (this->_command);	}
std::vector<std::string>	Client::getAgrs(void)	{	return (this->_args);	}
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

void	Client::setCommand(int command)	{	this->_command = command;	}
void	Client::setAgrs(std::vector<std::string> args)	{	this->_args = args;	}
/* ----- */

/*	/=/	*/
void	Client::appendToSendBuffer(std::string _send){	_sendbuffer += _send;	}
void	Client::clearArgs(void){	_args.clear();	}

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
