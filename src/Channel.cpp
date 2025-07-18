/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:42:47 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/18 16:27:33 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/colors.hpp"

//======================== CONSTRUCTORS ===========================//
// Default constructor
Channel::Channel()
{
	// Initialize the channel with default values
	_name = "";
	_topic = "";
	_password = "";
	_userLimit = 0;
	_key = "";
	// Initialize the modes map with default values
	_modes['i'] = false; // Invite-only mode
	_modes['t'] = false; // Topic settable by operators only
	_modes['k'] = false; // Key mode
	_modes['l'] = false; // User limit mode
	_modes['n'] = false; // No external messages mode
}

// Constructor with channel name and creator
Channel::Channel(const std::string &name, Client &createdBy) : _name(name), _userLimit(0)
{
	// Initialize the channel with the given name and default values
	_topic = "";
	_password = "";
	_key = "";
	// Initialize the modes map with default values
	_modes['i'] = false;					// Invite-only mode
	_modes['t'] = false;					// Topic settable by operators only
	_modes['k'] = false;					// Key mode
	_modes['l'] = false;					// User limit mode
	_modes['n'] = false;					// No external messages mode
	addUser(createdBy.getFd(), &createdBy); // Add the creator to the channel members
}

// Alena: copy and operator= must be disabled in irc project
/*
	Copy constructor
	Copying and assignment are disabled for this class.
 IRC objects must be unique and should never be copied.
*/
Channel::Channel(const Channel &other)
{
	logChannelError(" Copying a Channel is not allowed: IRC channels must be unique and non-copyable by design.");
	(void)other;
}

// Alena: copy and operator= must be disabled in irc project
/*
	Assigments operator.
	Copying and assignment are disabled for this class.
 IRC objects must be unique and should never be copied.
*/
Channel &Channel::operator=(const Channel &other)
{
	logChannelError(" Assignment of Channel is forbidden: IRC channels represent unique entities and cannot be duplicated.");
	(void)other;
	return *this;
}

// Destructor
Channel::~Channel()
{
	// No dynamic memory allocation, so nothing to clean up
	// The vectors and maps will be automatically cleaned up by the destructor
	logChannelInfo("Channel destructor called for: " + _name);
}

//======================== SETTERS ===================================//
// Function to set the topic of the channel
void Channel::setTopic(const std::string &topic) { _topic = topic; }

// Function to set a mode for the channel
// need to define a getter for the modes since is private attribute
void Channel::setMode(char mode, bool enable) { _modes[mode] = enable; }

// Function to set the name of the channel
void Channel::setName(std::string name) { _name = name; }

// Function to set a key for the channel
// need to define a getter for the key since is private attribute
void Channel::setKey(const std::string &key) { _key = key; }

// Function to set a limit for the channel
// need to define a getter for the userlimit since is private attribute
void Channel::setUserLimit(int limit)
{
	_userLimit = limit;
}

//======================== GETTERS ===================================//
std::string Channel::getNickList() const
{
	std::string list;

	for (std::map<int, Client *>::const_iterator it = _members.begin(); it != _members.end(); ++it)
	{
		Client *client = it->second;

		if (_operators.find(client) != _operators.end())
			list += "@" + client->getNickname();
		else
			list += client->getNickname();

		list += " ";
	}

	if (!list.empty())
		list.erase(list.end() - 1); // Remove trailing space

	return list;
}

// get the limit of users in the channel
int Channel::getLimit() const
{
	return _userLimit; // Return the user limit of the channel
}

// Function to get the name of the channel
std::string Channel::getName() const
{
	return _name; // Return the name of the channel
}

// Function to get the password of the channel
std::string Channel::getPassword() const
{
	return _password; // Return the password of the channel
}

// Function to get the topic of the channel
std::string Channel::getTopic() const
{
	return _topic; // Return the topic of the channel
}

// Function to get the key of the channel
std::string Channel::getKey() const
{
	return _key; // Return the key of the channel
}

// Function to get the members of the channel
std::map<int, Client *> Channel::getMembers() const
{
	return _members; // Return the vector of members in the channel
}

// Function to get the operators of the channel
const std::set<Client*>& Channel::getOperators() const
{
    return _operators;
}



std::string Channel::getModes() const
{
	std::string modes = "+";

	if (_modes.find('i') != _modes.end() && _modes.at('i'))
		modes += 'i';
	if (_modes.find('t') != _modes.end() && _modes.at('t'))
		modes += 't';
	if (!_key.empty())
		modes += 'k';
	if (_userLimit > 0)
		modes += 'l';

	if (modes == "+")
		return ""; // No active modes

	return modes;
}



//======================== MEMBER FUNCTIONS ===========================//

// Function to add a clients to the _members vector inside the channel
void Channel::addUser(int fd, Client *client)
{
	_members[fd] = client; // Add the client to the _members map using their file descriptor (fd) as the key
	logChannelInfo("Client added to channel: " + client->getNickname());
}

void Channel::addOperator(Client *client)
{
    _operators.insert(client);
    logChannelInfo("Client " + client->getNickname() + " was granted operator status");
}

void Channel::removeOperator(Client *client)
{
    _operators.erase(client);
    logChannelInfo("Client " + client->getNickname() + " lost operator status");
}


/*
This function checks if the user is an operator before removing them
- If the user is an operator, it prints an error message and does not remove them
- If the user is not an operator, it searches for the user in the _members vector and removes them if found
 */
void Channel::removeUser(int fd, Client *client)
{

	std::map<int, Client *>::iterator it = _members.find(fd);

	if (it != _members.end() && it->second == client)
	{
		_members.erase(it); // Erase the client from the _members map if found

		_operators.erase(client);
		_invited.erase(fd);

		logChannelInfo("[QUIT] Client left channel: " + client->getNickname());
	}
	else
	{
		logChannelInfo("Client not found in this channel:" + client->getNickname());
	}
}

// Function to check if a client is a member of the channel
bool Channel::hasMembers(Client *client) const
{
	std::map<int, Client *>::const_iterator it = _members.find(client->getFd());
	if (it != _members.end())
	{
		return true; // If the client is found in the members map, return true
	}
	else
	{
		return false; // If the client is not found, return false
	}
}

bool Channel::isOperator(Client *client) const
{
    if (!client)
        return false;

    std::map<int, Client *>::const_iterator it = _members.find(client->getFd());
    if (it == _members.end())
        return false; // Not a member

    return _operators.find(client) != _operators.end(); // Correct type
}


void Channel::makeOperator(Client *client)
{
	if (client && _members.find(client->getFd()) != _members.end())
		_operators.insert(client); //   correct: insert Client*
}

void Channel::sendToChannelAll(const std::string &message, Server &server)
{
	std::map<int, Client *>::iterator it;
	for (it = _members.begin(); it != _members.end(); ++it)
	{
		server.sendToClient(it->first, message);
	}
}

void Channel::sendToChannelExcept(const std::string &message, const Client &clientExcluded, Server &server) const
{
	for (std::map<int, Client *>::const_iterator it = _members.begin(); it != _members.end(); ++it)
	{
		int fd = it->first;
		if (fd == clientExcluded.getFd())
			continue;

		server.sendToClient(fd, message); // actually sends to socket
	}
}

// Function to check if the user's file descriptor (fd) is in the _invited set
bool Channel::isInvited(int fd) const
{
	return _invited.find(fd) != _invited.end(); // Return true if the user is invited, false otherwise
}

// Function to check if the channel is invite-only
bool Channel::isInviteOnly() const
{
	return _modes.at('i'); // Return true if the 'i' mode is enabled, false otherwise
}

void Channel::removeInvited(int fd)
{
	_invited.erase(fd);
	logChannelInfo("Removed invitation for fd " + intToString(fd) + " in channel: " + _name);
}

// Function to check if the topic is locked for changes
bool Channel::isTopicLocked() const
{
	return _modes.at('t'); // Return true if the 't' mode is enabled, false otherwise
}

// Function to check if the channel has a key/password set
bool Channel::hasKey() const
{
	return !_key.empty(); // Return true if the key is not empty, false otherwise
}

bool Channel::hasUserLimit() const
{
    std::map<char, bool>::const_iterator it = _modes.find('l');
    return it != _modes.end() && it->second;
}

//----------------------- COMMANDS HANDLERS -----------------------//
// Function to invite a user to the channel
void Channel::inviteUser(int fd)
{
	_invited.insert(fd); // Add the user's file descriptor (fd) to the _invited set
	logChannelInfo("User with fd " + intToString(fd) + " invited to channel: " + _name);
}
