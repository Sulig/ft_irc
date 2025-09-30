/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_structs.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 14:57:02 by sadoming          #+#    #+#             */
/*   Updated: 2025/09/30 19:57:35 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_STRUCTS_HPP

# include <poll.h>
# include <string>
# include <map>
# include <vector>

# define BACKLOG	20	// Max X persons in conexion queque

/*	Client	*/
typedef struct	s_client
{
	int	fd;
	std::string	buffer;
	std::string	nick;
	std::string	user;
}				t_client;

/*	GLOBAL SRUCT	*/
typedef struct	s_irc
{
	std::vector<pollfd>		fds;
	std::map<int, t_client>	clients;
}				t_irc;



#endif
