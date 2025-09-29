/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 15:00:31 by sadoming          #+#    #+#             */
/*   Updated: 2025/09/29 20:01:17 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP

# include <cstring>
# include <stdlib.h>
# include <sys/socket.h>
# include <netinet/in.h>

# include <iostream>

# include "irc_structs.hpp"

void	startServer(int);

#endif
