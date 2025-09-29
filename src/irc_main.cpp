/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_main.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 14:57:22 by sadoming          #+#    #+#             */
/*   Updated: 2025/09/29 19:01:49 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/irc.hpp"

int	main(int argc, const char **args)
{
	//* Basic input error handler: -->
	if (argc != 3)
	{
		std::cerr << "Error: Unexpected input." << std::endl;
		std::cerr << "Expected run: `./ircserv <port> <password>`" << std::endl;
		return (1);
	}

	int	port = std::atoi(args[1]);
	if (port <= 0 || port > 65535)
	{
		std::cerr << "Error: Wrong port number!" << std::endl;
		return (1);
	}

	std::string pass = args[2];
	if (pass.empty())
	{
		std::cerr << "Error: Password can't be empty!" << std::endl;
		return (1);
	}
	//*---------

	//* Run irc
	startServer(port);

	return (0);
}
