/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadoming <sadoming@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:31:58 by sadoming          #+#    #+#             */
/*   Updated: 2025/10/10 13:11:50 by sadoming         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include <string>

bool		isAllPrintable(std::string s);
std::string getCreationTime();
std::string	itoa(int n);
std::string	normalizeCommand(std::string input);

#endif
