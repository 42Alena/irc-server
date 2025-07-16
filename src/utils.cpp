/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 18:20:37 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/16 09:23:37 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.hpp"

void printWelcomeMessage()
{
	std::cout << std::endl;
	std::cout << std::endl;

	std::cout << RED << "🌟✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨🌟" << RST << std::endl;
	std::cout << ORG << "✨      W E L C O M E      T O   T H E          c I R C u s !               ✨" << RST << std::endl;
	std::cout << YEL << "✨                     Launch the  🎆magic below!                           ✨" << RST << std::endl;
	std::cout << RED << "🌟✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨🌟" << RST << std::endl;

	std::cout << std::endl;

	std::cout << CYN << "💫     🎟️   NICK  juggler               " << AQU << "// NICK <nickname>                 " << RST << std::endl;
	std::cout << BLU << "💫     🤡   USER  clown 0 * :BigTop      " << PNK << "// USER <username> <hostname>...   " << RST << std::endl;
	std::cout << MAG << "💫     🎪   JOIN  #tent                  " << RED << "// JOIN <#channel>                 " << RST << std::endl;

	std::cout << std::endl;
	std::cout << std::endl;

	std::cout << WHT << "                        🌟 Let the magic begin! 🌟            " << RST << std::endl;
	std::cout << ORG << "🌟✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨✨🌟" << RST << std::endl;

	std::cout << std::endl;
	std::cout << std::endl;
}

//============================LOG MESSAGES======================





/*
https://www.rfc-editor.org/rfc/rfc1459.html#section-2.3.1
"
<nick>       ::= <letter> { <letter> | <number> | <special> }

<letter>     ::= 'a' ... 'z' | 'A' ... 'Z'
<number>     ::= '0' ... '9'
<special>    ::= '-' | '[' | ']' | '\' | '`' | '^' | '{' | '}'
..
1.2 Clients
nickname having a maximum length of nine (9) characters.
"
https://cplusplus.com/reference/cctype/
*/
bool isValidNickname(const std::string &nickname)
{
	if (nickname.empty())
		return false;
	if (nickname.size() > 9)
		return false;

	if (!isalpha(nickname[0]))
		return false;

	for (std::string::size_type i = 1; i < nickname.size(); i++)
	{
		char c = nickname[i];
		if (!(isalpha(c) ||
			  isdigit(c) ||
			  c == '-' || c == '[' || c == ']' || c == '\\' || c == '`' || c == '^' || c == '{' || c == '}'))
			return false;
	}

	return true;
}

/*
- <username>: non-space, non-control characters (max 9 chars recommended)
<nonwhite>   ::= <any 8bit code except SPACE (0x20), NUL (0x0), CR
					 (0xd), and LF (0xa)>
 user       =  1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF )
				  ; any octet except NUL(\0), CR('\r'), LF('\n'), " " and "@"
*/
bool isValidUsername(const std::string &username)
{
	if (username.empty())
		return false;

	if (username.size() > 9)
		return false;

	for (std::string::size_type i = 1; i < username.size(); i++)
	{
		char c = username[i];
		if (c == '\0' || c == '\n' || c == '\r' || c == ' ' || c == '@' || !isprint(c))
			return false;
	}
	return true;
}

bool isValidPassword(const std::string &password)
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

std::string intToString(int n)
{
	std::ostringstream os;
	os << n;
	return os.str();
}