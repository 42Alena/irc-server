/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:20 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/27 17:33:24 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

//======================== PUBLIC: CONSTRUCTORS & DESTRUCTORS ==================//

Client::Client() : _fd(-1), _nickName(""), _userName(""), _registered(false), _receivedData("") {}

Client::Client(int fd) : _fd(fd), _nickName(""), _userName(""), _registered(false), _receivedData("") {}

Client::Client(int fd, std::string nickname, std::string receivedData)
    : _fd(fd), _nickName(nickname), _userName(""), _registered(false), _receivedData(receivedData) {}

Client::Client(const Client &o)
    : _fd(o._fd), _nickName(o._nickName), _userName(o._userName),
      _registered(o._registered), _channels(o._channels), _channelOps(o._channelOps),
      _sendData(o._sendData), _receivedData(o._receivedData) {}

Client &Client::operator=(const Client &o)
{
    if (this != &o)
    {
        _fd = o._fd;
        _nickName = o._nickName;
        _userName = o._userName;
        _registered = o._registered;
        _channels = o._channels;
        _channelOps = o._channelOps;
        _sendData = o._sendData;
        _receivedData = o._receivedData;
    }
    return *this;
}

Client::~Client() {}


//======================== PUBLIC: GETTERS =====================================//

int Client::getFd() const
{
    return _fd;
}

std::string Client::getNickname() const
{
    return _nickName;
}

std::string Client::getUserName() const
{
    return _userName;
}

const std::string &Client::getReceivedData() const
{
    return _receivedData;
}


//======================== PUBLIC: SETTERS =====================================//

void Client::setNickname(const std::string &nickname)
{
    _nickName = nickname;
}

void Client::setUser(const std::string &user)
{
    _userName = user;
}


//======================== PUBLIC: DATA BUFFER FUNCTIONS =======================//

void Client::appendToReceivedData(const std::string &chunk)
{
    _receivedData += chunk;
}

void Client::clearReceivedData()
{
    _receivedData.clear();
}


//======================== PUBLIC: COMMAND PARSING =============================//

bool Client::hasCompleteCommandRN()
{
    return _receivedData.find("\r\n") != std::string::npos;
}

std::string Client::extractNextCmd()
{
    std::string::size_type rn = _receivedData.find("\r\n");
    std::string extractedCommand = _receivedData.substr(0, rn);
    _receivedData.erase(0, rn + 2);
    return extractedCommand;
}


//======================== PUBLIC: CHANNEL FUNCTIONS ===========================//

bool Client::isRegistered() const
{
    return _registered;
}

bool Client::isOperator(const std::string &channel) const
{
    std::map<std::string, bool>::const_iterator it = _channelOps.find(channel);
    if (it != _channelOps.end())
        return it->second;
    return false;
}

void Client::joinChannel(const std::string &channel)
{
    _channels.push_back(channel);
}

void Client::leaveChannel(const std::string &channel)
{
    for (std::vector<std::string>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        if (*it == channel)
        {
            _channels.erase(it);
            break;
        }
    }
}

void Client::addOperator(const std::string &channel)
{
    _channelOps[channel] = true;
}

void Client::removeOperator(const std::string &channel)
{
    _channelOps[channel] = false;
}


//======================== NON-MEMBER OPERATORS ================================//

const std::ostream &operator<<(std::ostream &os, const Client &o)
{
    os << "Client(fd: " << o.getFd();
    os << ", nickname: " << o.getNickname();
    os << ", username: " << o.getUserName();
    os << ", receivedData: " << o.getReceivedData();
    os << ");" << std::endl;
    return os;
}
