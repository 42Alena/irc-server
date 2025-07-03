/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:20 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/03 20:13:25 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

//======================== PRIVATE: CONSTRUCTORS  (IRC clients are non-copyable)  ==============//

Client::Client(const Client &o)
{
    (void)o;
}

Client &Client::operator=(const Client &o)
{
    (void)o;
    return *this;
}

//======================== PUBLIC: CONSTRUCTORS & DESTRUCTORS ==================//
Client::Client()
    : _fd(-1),
      _nickName(""),
      _userName(""),
      _userModes(""),
      _hasProvidedPass(false),
      _hasProvidedNick(false),
      _hasProvidedUser(false),
      _channels(),
      _channelOps(),
      _sendData(""),
      _receivedData("")
{
}

Client::Client(int fd)
    : _fd(fd),
      _nickName(""),
      _userName(""),
      _userModes(""),
      _hasProvidedPass(false),
      _hasProvidedNick(false),
      _hasProvidedUser(false),
      _channels(),
      _channelOps(),
      _sendData(""),
      _receivedData("")
{
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

/*
Get list of channels, where this client is a member
 */
const std::vector<Channel *> &Client::getChannels() const
{
    return _channels;
}

bool Client::getHasProvidedPass() const
{
    return _hasProvidedPass;
}

bool Client::getHasProvidedNick() const
{
    return _hasProvidedNick;
}

bool Client::getHasProvidedUser() const
{
    return _hasProvidedUser;
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

void Client::setHasProvidedPass(bool val)
{
    _hasProvidedPass = val;
}

void Client::setHasProvidedNick(bool val)
{
    _hasProvidedNick = val;
}

void Client::setHasProvidedUser(bool val)
{
    _hasProvidedUser = val;
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
    return  _hasProvidedPass && _hasProvidedNick && _hasProvidedUser;
}

bool Client::isOperator(Channel *channel) const
{
    std::map<Channel *, bool>::const_iterator it = _channelOps.find(channel);
    if (it != _channelOps.end())
        return it->second;
    return false;
}

void Client::joinChannel(Channel *channel)
{
    _channels.push_back(channel);
}

void Client::leaveChannel(Channel *channel)
{
    std::vector<Channel *>::iterator it = _channels.begin();
    for (; it != _channels.end(); ++it)
    {
        if (*it == channel)
        {
            _channels.erase(it);
            break;
        }
    }
}

void Client::addOperator(Channel *channel)
{
    _channelOps[channel] = true;
}

void Client::removeOperator(Channel *channel)
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
