/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:04:34 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/07 14:08:06 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <cstring>
#include <string>
#include <algorithm>

std::string getCreationTime()
{
	time_t now = time(0);
	char* dt = ctime(&now);
	return std::string(dt);
}

std::string	normalizeCommand(std::string input)
{
	if (!input.empty())
		if (input[0] == '/' || input[0] == ':' || input[0] == ' ')
			input = input.substr(1);

	std::transform(input.begin(), input.end(), input.begin(), ::toupper);
	return (input);
}
