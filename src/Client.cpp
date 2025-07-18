/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:20 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/18 12:02:10 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

//======================== PRIVATE: CONSTRUCTORS  (IRC clients are non-copyable)  ===//

/*
	Copy constructor.
	Copying and assignment are disabled for this class.
	IRC clients represent unique socket connections and must not be copied.
*/
Client::Client(const Client &o)
{
    logClientError("Copying a Client is not allowed: IRC clients represent unique socket connections.");
    (void)o;
}

/*
	Assignment operator.
	Copying and assignment are disabled for this class.
	IRC clients represent unique socket connections and must not be duplicated.
*/
Client &Client::operator=(const Client &o)
{
    logClientError("Assignment of Client is forbidden: IRC clients represent unique socket connections.");
    (void)o;
    return *this;
}

//======================== PUBLIC: CONSTRUCTORS & DESTRUCTORS ==================//

Client::Client()
    : _fd(-1),
	  _host(""),
	  _nickname(""),
	  _username(""),
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


Client::Client(int fd, const std::string &host)
    : _fd(fd),
      _host(host),
      _nickname(""),
      _username(""),
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

Client::~Client() {
    logClientInfo("Client destructor called for fd " + intToString(_fd) + ", nick: " + _nickname);
	// No need to delete anything — all members are self-managed
}

//======================== PUBLIC: GETTERS =====================================//

int Client::getFd() const
{
    return _fd;
}

std::string Client::getNickname() const
{
    return _nickname;
}

std::string Client::getUsername() const
{
    return _username;
}

std::string Client::getRealname() const
{
    return _realname;
}

std::string Client::getHost() const
{
    return _host;
}

const std::string &Client::getReceivedData() const
{
    return _receivedData;
}

std::string Client::getUserModes() const
{
    return _userModes;
}

//Luis: added the implementation of the getter of the password from client to use in channel when joining
std::string Client::getPassword() const {
    return _password;
}

std::string Client::getPrefix() const
{
    
	return  _nickname + "!" + _username + "@" + _host;
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
    _nickname = nickname;
}

void Client::setUsername(const std::string &user)
{
    _username = user;
}

void Client::setRealname(const std::string &realname)
{
    _realname = realname;
}

void Client::addUserMode(char modeChar)
{
    if (_userModes.find(modeChar) == std::string::npos)
        _userModes += modeChar;
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
    return _hasProvidedPass && _hasProvidedNick && _hasProvidedUser;
}

bool Client::isOperator(Channel *channel) const
{
    std::map<Channel *, bool>::const_iterator it = _channelOps.find(channel);
    if (it != _channelOps.end())
        return it->second;
    return false;
}

bool Client::isInChannel(const std::string &channelName) const
{
    for (std::vector<Channel *>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        if ((*it)->getName() == channelName)
            return true;
    }
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

