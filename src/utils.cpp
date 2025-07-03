/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 18:20:37 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/03 18:28:42 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.hpp"


bool  isValidPassword(const std::string &password)
{
	if (password.empty())
		return false;
	if (password.size() > 32)
		return false;

	for (std::string::size_type i = 0; i < password.size(); i++)
	{
		char c = password[i];
		if (!isprint(c))
			return false;
	}

	return true;
}


std::string  intToString(int n)
{
	std::ostringstream os;
	os << n;
	return os.str();
}