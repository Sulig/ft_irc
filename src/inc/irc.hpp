/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 15:00:31 by sadoming          #+#    #+#             */
/*   Updated: 2025/09/30 20:13:06 by sadoming         ###   ########.fr       */
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
#endif
