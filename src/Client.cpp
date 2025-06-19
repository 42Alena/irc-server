/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:20 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/19 12:03:39 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client() : _fd(-1), _nickname(""), _receivedData("") {}

Client::Client(int fd) : _fd(fd) {}

Client::Client(int fd, std::string nickname, std::string receivedData) : _fd(fd),
																		 _nickname(nickname),
																		 _receivedData(receivedData) {}
Client::Client(const Client &o) : _fd(o._fd),
								  _nickname(o._nickname),
								  _receivedData(o._receivedData)
{
}

Client &Client::operator=(const Client &o)
{
	if (this != &o)
	{
		_fd = o._fd;
		_nickname = o._nickname;
		_receivedData = o._receivedData;
	}
	return *this;
}
Client::~Client() {}

//________________GETTERs_________________________________
int Client::getFd() const
{
	return _fd;
}

const std::string &Client::getNickname() const
{
	return _nickname;
}

const std::string &Client::getReceivedData() const
{
	return _receivedData;
}
//________________SETTERs_________________________________

void Client::setNickname(const std::string &nickname)
{
	_nickname = nickname;
}

void Client::appendToReceivedData(const std::string &chunk)
{
	_receivedData += chunk;
}

void Client::clearReceivedData()
{
	_receivedData.clear();
}

//________________OTHER FUNCTIONS_________________________________
/* Section 2.3 Messages
from IRC docs:
"IRC messages are always lines of characters terminated with a CR-LF
   (Carriage Return - Line Feed) pair, and these messages SHALL NOT
   exceed 512 characters in length, counting all characters including
   the trailing CR-LF. Thus, there are 510 characters maximum allowed
   for the command and its parameters.  There is no provision for
   continuation of message lines. ""
💡 CR-LF = \r\n

*/
bool Client::hasCompleteCommandRN()
{

	std::string::size_type n;
	n = _receivedData.find("\r\n");
	if (n == std::string::npos)
		return false;
	else
		return true;
}

std::string Client::extractNextCmd()
{

	std::string::size_type rn;
	rn = _receivedData.find("\r\n");


	std::string extractedCommand;
	extractedCommand = _receivedData.substr(0, rn);

	_receivedData.erase(rn + 2); // erase after rn to end
	
	return extractedCommand;
}

//________________operator<<_________________________________
const std::ostream &operator<<(std::ostream &os, const Client &o)
{

	os << "Client (fd: " << o.getFd();
	os << "Client (fd: " << o.getFd();
	//	os << ", nickname: " << o.getNickname() ; //for later
	os << ", receivedData: " << o.getReceivedData(); //for later
	os << ");" << std::endl;

	return os;
}