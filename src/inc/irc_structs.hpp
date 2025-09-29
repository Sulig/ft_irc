/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_structs.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 14:57:02 by sadoming          #+#    #+#             */
/*   Updated: 2025/09/29 17:17:08 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_STRUCTS_HPP

# include <string>

typedef	struct s_client
{
	int	fd;
	std::string	nick;
}				t_client;

#endif
