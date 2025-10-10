/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:04:34 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/10 13:11:15 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <cstring>
#include <sstream>
#include <string>
#include <algorithm>

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

bool	isAllPrintable(std::string s)
{
	for (size_t i = 0; i < s.size(); ++i)
	{
		unsigned char c = s[i];
		if (!std::isprint(c))
			return (false);
	}
	return (true);
}

std::string	normalizeCommand(std::string input)
{
	if (input.empty())
		return ("");

	std::transform(input.begin(), input.end(), input.begin(), ::toupper);
	return (input);
}
