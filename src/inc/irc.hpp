/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 15:00:31 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/06 13:44:25 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP

# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <unistd.h>

# include <cstring>
# include <iostream>
# include <stdexcept>

# include "colors.hpp"
# include "irc_structs.hpp"

void	startServer(int port, std::string pass);
void	serverLoop(t_irc *irc);

void	handleNewClient(int server_fd, t_irc *irc);
void	readClientData(t_irc *irc, size_t pos, int client_fd);
void	handleClientExit(t_irc *irc, size_t pos, int client_fd);
#endif
