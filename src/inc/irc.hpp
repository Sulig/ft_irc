/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 15:00:31 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/01 13:04:21 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP

# include <cstring>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <unistd.h>

# include <iostream>

# include "colors.hpp"
# include "irc_structs.hpp"

void	startServer(int port, std::string pass);
void	serverLoop(int server_fd, std::string pass);

void	handleNewClient(int server_fd, t_irc irc);
void	handleClientData(t_irc irc, size_t pos, std::string pass);
#endif
