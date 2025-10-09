/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 16:26:01 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/09 18:53:29 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>

# define	NICK_MAX_CHARS			9
# define	NICK_MUST_NOT_CONTAIN	" ,*?!@.;"
# define	NICK_MUST_NOT_STARTWITH	"$:#&~"

class	Client
{
	private:
		int		_fd;
		bool	_is_logged;
		bool	_is_welcomeSend;
		size_t	_pos;

		size_t	_command;
		std::vector<std::string>	_args;

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
		bool	getIsWelcomeSend(void);
		std::string	getBuffer(void);
		std::string	getSendBuffer(void);
		std::string	getNick(void);
		std::string	getUser(void);

		int		getCommand(void);
		std::vector<std::string>	getAgrs(void);

		/*	SETTERS	*/
		void	setPos(size_t pos);
		void	setIsLogged(bool logged);
		void	setIsWelcomeSend(bool welcome);
		void	setBuffer(std::string buffer);
		void	setSendBuffer(std::string _send);
		void	setNick(std::string nick);
		void	setUser(std::string user);

		void	setCommand(int command);
		void	setAgrs(std::vector<std::string> args);

		//** */
		void	appendToSendBuffer(std::string _send);
		void	clearArgs(void);
		int	sendPendingData(void);
};

#endif
