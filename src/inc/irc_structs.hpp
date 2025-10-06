/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_structs.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 14:57:02 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/06 14:34:54 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_STRUCTS_HPP

# include <poll.h>
# include <string>
# include <map>
# include <vector>

# define BACKLOG	20	// Max X persons in conexion queque

/*	COMMANDS	*/
# define QUIT	0
# define PASS	1
# define NICK	2
# define PRIVMSG	3

# define JOIN	'J'
# define PART	'P'
# define KICK	'K'
# define INVITE	'I'
# define TOPIC	'T'
# define MODE	'M'

# define PING	'\p'
# define PONG	'\t'

/*	Client	*/
typedef struct	s_client
{
	int			fd;
	bool		is_logged;
	std::string	buffer;
	std::string	nick;
	std::string	user;
}				t_client;

/*	GLOBAL SRUCT	*/
typedef struct	s_irc
{
	int						server_fd;
	std::vector<pollfd>		fds;
	std::map<int, t_client>	clients;
	std::string				pass;
}				t_irc;



#endif
