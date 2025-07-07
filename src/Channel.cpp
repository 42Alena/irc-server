/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luifer <luifer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:42:47 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/08 00:51:38 by luifer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/colors.hpp"

// TODO (Alena):: commented out, "_limit" is not a nonstatic data member or base class of class "Channel"C/C++(292)
// constructor with channel name
// Channel::Channel(const std::string& name) : _name(name), _limit(0) {}

//======================== CONSTRUCTORS ===========================//
// Default constructor
Channel::Channel() {
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
Channel::Channel(const std::string &name, Client &createdBy) : _name(name), _userLimit(0) {
	// Initialize the channel with the given name and default values
	_topic = "";
	_password = "";
	_key = "";
	// Initialize the modes map with default values
	_modes['i'] = false; // Invite-only mode
	_modes['t'] = false; // Topic settable by operators only
	_modes['k'] = false; // Key mode
	_modes['l'] = false; // User limit mode
	_modes['n'] = false; // No external messages mode
	addUser(createdBy.getFd(), &createdBy); // Add the creator to the channel members
}

// Copy constructor
Channel::Channel(const Channel &other) {
	// Copy the attributes from the other channel
	_name = other._name;
	_topic = other._topic;
	_password = other._password;
	_members = other._members;
	_operators = other._operators;
	_modes = other._modes;
	_key = other._key;
	_userLimit = other._userLimit;
}

// Assignment operator
Channel &Channel::operator=(const Channel &other) {
	if (this != &other) {
		// Check for self-assignment
		_name = other._name; // Copy the name of the channel
		_topic = other._topic; // Copy the topic of the channel
		_password = other._password; // Copy the password of the channel
		_members = other._members; // Copy the members of the channel
		_operators = other._operators; // Copy the operators of the channel
		_modes = other._modes; // Copy the modes of the channel
		_key = other._key; // Copy the key of the channel
		_userLimit = other._userLimit; // Copy the user limit of the channel
	}
	return *this; // Return a reference to this object
}

// Destructor
Channel::~Channel() {
	// No dynamic memory allocation, so nothing to clean up
	// The vectors and maps will be automatically cleaned up by the destructor
	std::cout << RED << "Channel " << _name << " destroyed." << RST << std::endl;
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
void Channel::setLimit(int limit) {
	_userLimit = limit;
}


//======================== GETTERS ===================================//

//get the limit of users in the channel
int Channel::getLimit() const {
	return _userLimit; // Return the user limit of the channel
}

// Function to get the name of the channel
std::string Channel::getName() const {
	return _name; // Return the name of the channel
}

// Function to get the password of the channel
std::string Channel::getPassword() const {
	return _password; // Return the password of the channel
}

// Function to get the topic of the channel
std::string Channel::getTopic() const {
	return _topic; // Return the topic of the channel
}

// Function to get the key of the channel
std::string Channel::getKey() const {
	return _key; // Return the key of the channel
}

// Function to get the members of the channel
std::map<int, Client *> Channel::getMembers() const {
	return _members; // Return the vector of members in the channel
}

// Function to get the operators of the channel
std::set<int> Channel::getOperators() const {
	return _operators; // Return the vector of operators in the channel
}


//======================== MEMBER FUNCTIONS ===========================//

// Function to add a clients to the _members vector inside the channel
void Channel::addUser(int fd, Client *client) {
	_members[fd] = client; // Add the client to the _members map using their file descriptor (fd) as the key
	std::cout << BLU << "Client added to channel: " << client->getNickname() << RST << std::endl; 
}

// Function to add a client to the _operators vector inside the channel
void Channel::addOperator(int fd) {	
	_operators.insert(fd); // Add the client's file descriptor (fd) to the _operators set
	std::cout << BLU << "Client added as operator: " << fd << RST << std::endl;
}

// This function checks if the user is an operator before removing them
// If the user is an operator, it prints an error message and does not remove them
// If the user is not an operator, it searches for the user in the _members vector and removes them if found
void Channel::removeUser(int fd, Client *client)
{
	if (isOperator(client))
	{
		std::cout << RED << "Error: Cannot remove operator from channel." << RST << std::endl;
	}
	std::map<int, Client *>::iterator it = _members.find(fd);
	if(it != _members.end() && it->second == client){
		_members.erase(it); // Erase the client from the _members map if found
		std::cout << RED << "Client removed from channel: " << client->getNickname() << RST << std::endl;
	}
	else {
		std::cout << BLU << "Client not found in this channel, are you sure is here?." << RST << std::endl;
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

// Function to check if a client is an operator of the channel
bool Channel::isOperator(Client *client) const
{
	std::map<int, Client *>::const_iterator it = _members.find(client->getFd());
	if(it == _members.end())
		return false; // If the client is not found in the members map, return false
	else if (_operators.find(client->getFd()) != _operators.end())
		return true; // If the client is found in the operators set, return true)
	else
		return false; // If the client is not found in the operators set, return false
}

// Function to broadcast a message to all members of the channel
void Channel::broadCastMessage(const std::string &message, int excludeFd) const {
    // Iterate through the _members map
    for (std::map<int, Client *>::const_iterator it = _members.begin(); it != _members.end(); ++it) {
        int fd = it->first;         // Get the file descriptor
        Client *client = it->second; // Get the client pointer
        // Skip the sender (excludeFd)
        if (fd == excludeFd) {
            continue;
        }
        // Send the message to the client
        client->appendToReceivedData(message);
    }
}

// Function to check if the user's file descriptor (fd) is in the _invited set
bool Channel::isInvited(int fd) const {
	return _invited.find(fd) != _invited.end(); // Return true if the user is invited, false otherwise
}

// Function to check if the channel is invite-only
bool Channel::isInviteOnly() const {
	return _modes.at('i'); // Return true if the 'i' mode is enabled, false otherwise
}

// Function to check if the topic is locked for changes
bool Channel::isTopicLocked() const {
	return _modes.at('t'); // Return true if the 't' mode is enabled, false otherwise
}

// Function to check if the channel has a key/password set
bool Channel::hasKey() const {
	return !_key.empty(); // Return true if the key is not empty, false otherwise
}

// Function to check if the channel has a user limit set
bool Channel::hasUserLimit() const {
	return _modes.at('l'); // Return true if the 'l' mode is enabled, false otherwise
}

//----------------------- COMMANDS HANDLERS -----------------------//
//Function to invite a user to the channel
void Channel::inviteUser(int fd) {
	_invited.insert(fd); // Add the user's file descriptor (fd) to the _invited set
	std::cout << BLU << "User with fd " << fd << " invited to channel: " << _name << RST << std::endl;
}

// Function to handle a user joining the channel
//it check if the channel is not created and created it if needed 
//it check if the user is already a member of the channel
//it check if the channel is invite-only and if the user is invited
//it check if the channel has a key and if the user provided the correct key
//it check if the channel has a user limit and if it is reached
//after all checks, it adds the user to the channel and broadcasts a message to other members
void Channel::handleJoin(Client *client, Server &server) {
	if (!server.isChannelName(_name)){
		//Channel *newChannel = new Channel(_name, *client); // Create a new channel with the client's nickname
		// ToDo -> when defined the method to add a new channel in Server class, call it here
		std::cout << BLU << "Channel " << _name << " created by " << client->getNickname() << RST << std::endl;
	}
	if (hasMembers(client)) {
		std::cout << RED << "Error: User already in channel." << RST << std::endl;
		return; // If the user is already a member, return with an error message
	}
	if (isInviteOnly() && !isInvited(client->getFd())) {
		std::cout << RED << "Error: Channel is invite-only." << RST << std::endl;
		return; // If the channel is invite-only and the user is not invited, return with an error message
	}
	if (hasKey() && client->getPassword() != _key) {
		std::cout << RED << "Error: Incorrect channel key." << RST << std::endl;
		return; // If the channel has a key and the user's password does not match, return with an error message
	}
	if (hasUserLimit() && static_cast<int>(_members.size()) >= _userLimit) {
		std::cout << RED << "Error: Channel user limit reached." << RST << std::endl;
		return; // If the channel has a user limit and it is reached, return with an error message
	}
	addUser(client->getFd(), client); // Add the user to the channel
	broadCastMessage(client->getNickname() + " has joined the channel.", client->getFd()); // Notify other members
	std::cout << BLU << "User " << client->getNickname() << " joined channel: " << _name << RST << std::endl;
}

// Function to handle a user leaving the channel
void Channel::handlePart(Client *client, Server &server) {
	(void)server; // Unused parameter for now
	if (!hasMembers(client)) {
		std::cout << RED << "Error: User not in channel." << RST << std::endl;
		return; // If the user is not a member, return with an error message
	}
	removeUser(client->getFd(), client); // Remove the user from the channel
	broadCastMessage(client->getNickname() + " has left the channel.", client->getFd()); // Notify other members
	std::cout << BLU << "User " << client->getNickname() << " left channel: " << _name << RST << std::endl;
}

// Function to handle the change of the topic in the channel
// it checks if the user is an operator before allowing them to change the topic
// after this check if set the new topic and broadcasts a message to other members
void Channel::handleTopic(Client *client, const std::string &newTopic) {
	if (!isOperator(client)) {
		std::cout << RED << "Error: Only operators can change the topic." << RST << std::endl;
		return; // If the user is not an operator, return with an error message
	}
	setTopic(newTopic); // Use the setter to change the topic
	broadCastMessage("Topic changed to: " + _topic, client->getFd()); // Notify other members
	std::cout << BLU << "Topic changed to: " << _topic << " by " << client->getNickname() << RST << std::endl;
}

// Function to handle the change of modes in the channel
// it checks if the user is an operator before allowing them to change modes
// after this check it sets the mode and captures the boolean enable to determine if the mode is being enabled or disabled
// and then broadcasts a message to other members notifying them of the change
void Channel::handleMode(Client *client, char mode, bool enable) {
	if (!isOperator(client)) {
		std::cout << RED << "Error: Only operators can change modes." << RST << std::endl;
		return; // If the user is not an operator, return with an error message
	}
	setMode(mode, enable); // Use the setter to change the mode
	std::string modeStatus = enable ? "enabled" : "disabled";
	broadCastMessage("Mode '" + std::string(1, mode) + "' has been " + modeStatus + ".", client->getFd()); // Notify other members
	std::cout << BLU << "Mode '" << mode << "' has been " << modeStatus << " by " << client->getNickname() << RST << std::endl;
}

// Function to handle inviting a user to the channel
// it checks if the user is an operator before allowing them to invite others
// if the user is already invited, it returns with an error message
// if the user is not invited, it adds them to the invited list and broadcasts a message to other members
void Channel::handleInvite(Client *client, int targetFd) {
	if (!isOperator(client)) {
		std::cout << RED << "Error: Only operators can invite users." << RST << std::endl;
		return; // If the user is not an operator, return with an error message
	}
	if (isInvited(targetFd)) {
		std::cout << RED << "Error: User already invited." << RST << std::endl;
		return; // If the user is already invited, return with an error message
	}
	inviteUser(targetFd); // Add the user to the invited list
	broadCastMessage(client->getNickname() + " has invited a user to the channel.", client->getFd()); // Notify other members
	std::cout << BLU << "User with fd " << targetFd << " invited to channel: " << _name << RST << std::endl;
}

// Function to handle kicking a user from the channel
void Channel::handleKick(Client *client, int targetFd, const std::string &reason) {
	if (!isOperator(client)) {
		std::cout << RED << "Error: Only operators can kick users." << RST << std::endl;
		return; // If the user is not an operator, return with an error message
	}
	if (!hasMembers(client)) {
		std::cout << RED << "Error: User not in channel." << RST << std::endl;
		return; // If the user is not a member, return with an error message
	}
	removeUser(targetFd, client); // Remove the user from the channel
	broadCastMessage(client->getNickname() + " has kicked a user from the channel. Reason: " + reason, client->getFd()); // Notify other members
	std::cout << BLU << "User with fd " << targetFd << " kicked from channel: " << _name << RST << std::endl;
}

// Function to handle setting a key for the channel
void Channel::handleKey(Client *client, const std::string &key) {
	if (!isOperator(client)) {
		std::cout << RED << "Error: Only operators can set a key." << RST << std::endl;
		return; // If the user is not an operator, return with an error message
	}
	setKey(key); // Use the setter to change the key
	broadCastMessage("Channel key has been changed", client->getFd()); // Notify other members
	std::cout << BLU << "Channel key has been set by " << client->getNickname() << RST << std::endl;
}