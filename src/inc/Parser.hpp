/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 14:23:32 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/28 14:31:20 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include <vector>

enum CommandType
{
	HELP,
	PASS,
	CAP,
	NICK,
	USER,
	PRIVMSG,
	QUIT,
	JOIN,
	PART,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	PING,
	PONG,
	STAT,
	CLEAR,
	UNKNOWN
};

typedef struct s_cmd
{
	CommandType					type;
	std::vector<std::string>	args;
}								t_cmd;


#endif
