/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:20 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/26 13:07:46 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client() : _fd(-1), _nickName(""), _receivedData("") {}

Client::Client(int fd) : _fd(fd) {}

Client::Client(int fd, std::string nickname, std::string receivedData) : _fd(fd),
																		 _nickName(nickname),
																		 _receivedData(receivedData) {}
Client::Client(const Client &o) : _fd(o._fd),
								  _nickName(o._nickName),
								  _receivedData(o._receivedData)
{
}

Client &Client::operator=(const Client &o)
{
	if (this != &o)
	{
		_fd = o._fd;
		_nickName = o._nickName;
		_receivedData = o._receivedData;
	}
	return *this;
}
Client::~Client() {}

//________________GETTERs_________________________________
//to get the file descriptor of the client
int Client::getFd() const
{
	return _fd;
}

//to get the nickname of the client
std::string Client::getNickname() const
{
	return _nickName;
}

//to get the username of the client
std::string Client::getUser() const
{
	return _user;
}

const std::string &Client::getReceivedData() const
{
	return _receivedData;
}
//________________SETTERs_________________________________

//to set the nickname to the client
void Client::setNickname(const std::string &nickname)
{
	_nickName = nickname;
}

//to set the user to the client
void Client::setUser(const std::string &user)
{
	_user = user;
}

//to append the received data from the client to the existing data
void Client::appendToReceivedData(const std::string &chunk)
{
	_receivedData += chunk;
}

void Client::clearReceivedData()
{
	_receivedData.clear();
}

//join a channel
void Client::joinChannel(const std::string& channel){
	_channels.push_back(channel);
}

//leave a channel
void Client::leaveChannel(const std::string& channel){
	
	(void) channel;
	//ToDo...
	//...
	//remove the channel from the vector of channels
	//...
	//ToDo...
}


//Add an operator to the channel
void Client::addOperator(const std::string& channel){
	_channelOps[channel] = true;
}

//Remove an operator from the channel
void Client::removeOperator(const std::string& channel){
	_channelOps[channel] = false;
}

//check if a client is an operator in a channel
bool Client::isOperator(const std::string& channel) const{
	// Check if the channel exists in the map
	if (_channelOps.find(channel) != _channelOps.end()){
		return _channelOps.at(channel);
	}
	return false;
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