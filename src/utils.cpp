/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:04:34 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/09 19:21:23 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <cstring>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

std::string	getCreationTime()
{
	time_t now = time(0);
	char* dt = ctime(&now);
	return std::string(dt);
}

std::string	itoa(int n)
{
	std::ostringstream oss;
	oss << n;
	return oss.str();
}

std::string	normalizeCommand(std::string input)
{
	if (input.empty())
		return ("");

	if (input[0] == '/' || input[0] == '\\' || input[0] == ' ')
		input = input.substr(1);

	std::transform(input.begin(), input.end(), input.begin(), ::toupper);
	return (input);
}
