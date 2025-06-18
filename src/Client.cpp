/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:20 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/18 12:27:55 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client() : _fd(-1), _nickname(""), _receivedData("") {}

Client::Client(int fd): _fd(fd){}

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
Client::~Client(){}

//________________getter_________________________________
int Client::getFd() const
{
	return _fd;
}

const std::string &Client::getNickname() const {
	return _nickname;
}

const std::string &Client::getReceivedData() const {
	return _receivedData;
}

const std::ostream &operator<<(std::ostream &os, const Client &o) {

	os << "Client (fd: "   << o.getFd();
//	os << ", nickname: " << o.getNickname() ; //for later
//	os << ", receivedData: " << o.getReceivedData(); //for later
	os << ");"  << std::endl;
	
	return os;
}