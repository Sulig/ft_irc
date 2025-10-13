/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 16:26:01 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/13 19:17:58 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>

/*	NICK RULES	*/
# define	NICK_MAX_CHARS			9
# define	NICK_MUST_NOT_CONTAIN	" ,*?!@.;"
# define	NICK_MUST_NOT_STARTWITH	"$:#&~"

/*	USER RULES	*/
# define	USER_MUST_NOT_CONTAIN	" :"

/*	CLIENT MODES	*/
# define	MODE_DEF	0x00
# define	MODE_W		0x02
# define	MODE_I		0x04

class	Client
{
	private:
		int		_fd;
		bool	_is_logged;
		bool	_is_registered;
		bool	_is_welcomeSend;
		size_t	_pos;

		size_t	_command;
		std::vector<std::string>	_args;

		std::string	_buffer;
		std::string	_sendbuffer;
		std::string	_nick;
		std::string	_user;
		std::string	_realname;
		int			_userModes;

		//*	CHANNEL	*/

		/*	PING - PONG	*/
		time_t		_lastPingSent;
		time_t		_lastPongSent;
		time_t		_lastActivity;
		bool		_isPongSent;
		bool		_isPongWaiting;

		void	clientStartVars(void);

	public:
		Client();
		Client(int fd);
		~Client();

		/*	GETTERS	*/
		size_t	getPos(void);
		bool	getIsLogged(void);
		bool	getIsRegistered(void);
		bool	getIsWelcomeSend(void);
		std::string	getBuffer(void);
		std::string	getSendBuffer(void);
		std::string	getNick(void);
		std::string	getUser(void);
		std::string	getRealName(void);

		int		getUserModes(void);

		bool	getIsPongSent(void);
		bool	getIsPongWaiting(void);
		time_t	getLastPingSent(void);
		time_t	getLastPongSent(void);
		time_t	getLastActivity(void);

		int		getCommand(void);
		std::vector<std::string>	getAgrs(void);

		/*	SETTERS	*/
		void	setPos(size_t pos);
		void	setIsLogged(bool logged);
		void	setIsRegistered(bool registerMe);
		void	setIsWelcomeSend(bool welcome);
		void	setBuffer(std::string buffer);
		void	setSendBuffer(std::string _send);
		void	setNick(std::string nick);
		void	setUser(std::string user);
		void	setRealName(std::string name);

		void	setUserModes(int modes);

		void	setIsPongSent(bool pong);
		void	setIsPongWaiting(bool pong);
		void	setLastPingSent(time_t time);
		void	setLastPongSent(time_t time);
		void	setLastActivity(time_t time);

		void	setCommand(int command);
		void	setAgrs(std::vector<std::string> args);

		/** */
		void	appendToSendBuffer(std::string _send);
		void	clearArgs(void);
		int		sendPendingData(void);
};

#endif
