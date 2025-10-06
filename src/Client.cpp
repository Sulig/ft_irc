/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 13:42:52 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/06 17:47:33 by sadoming         ###   ########.fr       */
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
/* ----- */
