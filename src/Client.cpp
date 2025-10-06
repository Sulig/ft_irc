/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 13:42:52 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/06 20:11:24 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "inc/Client.hpp"
# include "inc/Server.hpp"

/* Constructor & destructor */
Client::Client()	{	_fd = 0; _is_logged = false;	}
Client::Client(int fd)	{	this->_fd = fd; _is_logged = false;	}
Client::~Client()	{}
/* ----- */

/* GETTERS */
std::string Client::getBuffer(void)	{	return (this->_buffer);	}
/* ----- */

/*	/=/	*/
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
	if (errno == EAGAIN || errno == EWOULDBLOCK)
		return (-1);
	else
		return (-2);
}
/* ----- */
