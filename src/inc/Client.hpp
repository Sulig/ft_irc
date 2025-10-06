/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 16:26:01 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/06 17:46:50 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class	Client
{
	private:
		int		_fd;
		bool	_is_logged;
		std::string	_buffer;
		std::string	_nick;
		std::string	_user;

	public:
		Client();
		Client(int fd);
		~Client();

		/*	GETTERS	*/
		std::string	getBuffer(void);

		//** */
};

#endif
