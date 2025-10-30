#include <algorithm>
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 18:51:10 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/30 18:51:12 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/Parser.hpp"
#include <ctime>
#include <cstring>
#include <sstream>
#include <string>
#include <algorithm>
#include <iostream>

std::string	Parser::normalizeCommand(std::string input)
{
	if (input.empty())
		return ("");

	size_t	found = input.find_first_of(' ');
	if (found != std::string::npos)
		input = input.substr(0, found);

	std::transform(input.begin(), input.end(), input.begin(), ::toupper);
	return (input);
}

cmdType Parser::identifyCmd(std::string cmd)
{
	if (cmd.empty())
		return (UNKNOWN);

	std::string normalized = normalizeCommand(cmd);

	if (normalized.compare(0, 4, "HELP") == 0) return HELP;
	if (normalized.compare(0, 4, "PASS") == 0) return PASS;
	if (normalized.compare(0, 4, "NICK") == 0) return NICK;
	if (normalized.compare(0, 4, "USER") == 0) return USER;
	if (normalized.compare(0, 7, "PRIVMSG") == 0) return PRIVMSG;
	if (normalized.compare(0, 4, "QUIT") == 0) return QUIT;
	if (normalized.compare(0, 4, "JOIN") == 0) return JOIN;
	if (normalized.compare(0, 4, "PART") == 0) return PART;
	if (normalized.compare(0, 4, "KICK") == 0) return KICK;
	if (normalized.compare(0, 6, "INVITE") == 0) return INVITE;
	if (normalized.compare(0, 5, "TOPIC") == 0) return TOPIC;
	if (normalized.compare(0, 4, "MODE") == 0) return MODE;
	if (normalized.compare(0, 4, "PING") == 0) return PING;
	if (normalized.compare(0, 4, "PONG") == 0) return PONG;
	if (normalized.compare(0, 5, "CLEAR") == 0) return CLEAR;
	if (normalized.compare(0, 4, "STAT") == 0) return STAT;
	if (normalized.compare(0, 3, "CAP") == 0) return CAP;

	return UNKNOWN;
}

std::vector<std::string>	Parser::parseArgs(std::string input)
{
	std::vector<std::string>	args;
	size_t	pos = 0, last = 0;

	if (input.find("\n") != std::string::npos)
		input = input.substr(0, input.find("\n"));
	while (pos < input.size())
	{
		pos = input.find_first_not_of(' ', pos);
		if (pos < input.size())
		{
			if (input[pos] == ':')
			{
				args.push_back(input.substr(pos + 1));
				break ;
			}
			last = input.find_first_of(' ', pos);
			if (last > input.size())
				last = input.size();
			args.push_back(input.substr(pos, last - pos));
			pos = last + 1;
		}
	}
	return (args);
}

t_cmd	Parser::parse(std::string msg)
{
	t_cmd	cmd;
	cmd.raw_line = msg;

	// Cleaning --
	while (!msg.empty())
	{
		if (msg[0] == '/' || msg[0] == '\\' || msg[0] == ' ')
			msg = msg.substr(1);
		else
			break;
	}

	cmd.type = identifyCmd(msg);
	// Remove the command from message
	if (cmd.type != UNKNOWN)
	{
		int	cmd_lenght = 0;
		switch (cmd.type)
		{
			case CAP: cmd_lenght = 3; break;
			case INVITE: cmd_lenght = 6; break;
			case PRIVMSG: cmd_lenght = 7; break;
			case HELP: case PASS: case NICK: case USER: case QUIT: case JOIN: cmd_lenght = 4; break;
			case PART: case KICK: case MODE: case PING: case PONG: case STAT: cmd_lenght = 4; break;
			case TOPIC: case CLEAR: cmd_lenght = 5; break;
			default: cmd_lenght = 0; break;
		}
		msg = msg.substr(cmd_lenght);
		std::cout << "|| -- parser -- msg: " << msg << "|" << std::endl;
		cmd.args = parseArgs(msg);
	}
	return (cmd);
}
