/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 16:26:01 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/07 13:35:08 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class	Client
{
	private:
		int		_fd;
		size_t	_pos;
		bool	_is_logged;
		int		_command;
		std::string	_buffer;
		std::string	_sendbuffer;
		std::string	_nick;
		std::string	_user;

	public:
		Client();
		Client(int fd);
		~Client();

		/*	GETTERS	*/
		size_t	getPos(void);
		bool	getIsLogged(void);
		int		getCommand(void);
		std::string	getBuffer(void);
		std::string	getSendBuffer(void);
		std::string	getNick(void);

		/*	SETTERS	*/
		void	setPos(size_t pos);
		void	setIsLogged(bool logged);
		void	setCommand(int command);
		void	setBuffer(std::string buffer);
		void	setSendBuffer(std::string _send);
		void	setNick(std::string nick);

		//** */
		void	appendToSendBuffer(std::string _send);
		int	sendPendingData(void);
};

#endif
