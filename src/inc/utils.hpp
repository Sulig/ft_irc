/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:31:58 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/14 13:05:55 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include <string>
#include <vector>

typedef struct s_command
{
	size_t						cmd_num;
	std::vector<std::string>	args;
}								t_command;

bool		isAllPrintable(std::string s);
std::string getCreationTime();
std::string	itoa(int n);
std::string	normalizeCommand(std::string input);

#endif
