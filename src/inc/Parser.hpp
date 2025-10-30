/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 14:23:32 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/30 20:02:51 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include <vector>

enum cmdType
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
	std::string					raw_line;
	cmdType						type;
	std::vector<std::string>	args;
}								t_cmd;

class Parser
{
	private:
		static std::string	normalizeCommand(std::string cmd);

	public:
		Parser(/* args */);
		~Parser();

		static t_cmd	parse(std::string message);
		static cmdType	identifyCmd(std::string cmd);
		static std::vector<std::string>	parseArgs(std::string input);
};

#endif
