/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 16:26:01 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/28 13:32:26 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>
# include <set>
# include <ctime>

# include "utils.hpp"

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
		bool	_is_irssi;
		bool	_is_logged;
		bool	_is_registered;
		bool	_is_welcomeSend;
		size_t	_pos;

		/*	COMMAND	*/
		t_command				_actualcmd;

		/*	CLIENT REGISTER VARS	*/
		std::string	_buffer;
		std::string	_sendbuffer;
		std::string	_nick;
		std::string	_user;
		std::string	_realname;
		int			_userModes;

		//*	CHANNEL	*/
		std::set<std::string>	channel_name;

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
		size_t	getFD(void);
		size_t	getPos(void);
		bool	getIsIrrsi(void);
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

		std::vector<std::string>	getActualCmdArgs(void);
		t_command					getActualCommand(void);

		/*	SETTERS	*/
		void	setPos(size_t pos);
		void	setIsIrrsi(bool irssi);
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

		void	setActualCommand(t_command cmd);

		//** */
		void	appendToSendBuffer(std::string _send);
		int		sendPendingData(void);

		/*	CHANNELS	*/
		void	addChannel(const std::string& name);
		void	removeChannel(const std::string& name);
		bool	inChannel(const std::string& name) const;
		const	std::set<std::string>& channels() const;
};

#endif
