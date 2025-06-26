/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:42:47 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/26 15:06:31 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Channel.hpp"

// TODO (Alena):: commented out, "_limit" is not a nonstatic data member or base class of class "Channel"C/C++(292)
// constructor with channel name
// Channel::Channel(const std::string& name) : _name(name), _limit(0) {}

// Function to add a user to the channel
void Channel::addUser(Client *client)
{

	// TODO(Alena) : made tmp void for compiling ( unused parameter 'client')
	(void)client;

	// Add clients to the _members vector
}

// Function to remove a user from the channel
void Channel::removeUser(Client *client)
{

	// TODO(Alena) : made tmp void for compiling ( unused parameter 'client')
	(void)client;

	// Remove clients from the _members vector
}

// Function to set the topic of the channel
void Channel::setTopic(const std::string &topic) { _topic = topic; }

// Function to set a mode for the channel
// need to define a getter for the modes since is private attribute
void Channel::setMode(char mode, bool enable) { _modes[mode] = enable; }

// Function to set a key for the channel
// need to define a getter for the key since is private attribute
void Channel::setKey(const std::string &key) { _key = key; }

// Function to set a limit for the channel
// need to define a getter for the userlimit since is private attribute
void Channel::setLimit(int limit) { _userLimit = limit; }

/*
TODO(Alena): commented out, because same function line 97 -115
   79 | bool Channel::hasMembers(Client* client) const{
	  |               ^
src/Channel.cpp:56:15: note: previous definition is here
   56 | bool Channel::hasMembers(Client* client) const{
	  |               ^
src/Channel.cpp:89:15: error: redefinition of 'isOperator'
   89 | bool Channel::isOperator(Client* client) const {
	  |               ^
src/Channel.cpp:67:15: note: previous definition is here
   67 | bool Channel::isOperator(Client* client) const {
	  |




//Function to check if a client is a member of the channel
bool Channel::hasMembers(Client* client) const{

		//TODO(Alena) : made tmp void for compiling ( unused parameter 'client')
		(void)client;

	//return true if _client is in _members vector

	//TODO(Alena) : made tmp return true for compiling ( unused parameter 'client')
	return true;
}

bool Channel::isOperator(Client* client) const {

	//TODO(Alena) : made tmp void for compiling ( unused parameter 'client')
	(void)client;

	//return true if _client is in _operators vector

		//TODO(Alena) : made tmp return true for compiling ( unused parameter 'client')
		return true;
}
*/

// Function to check if a client is a member of the channel
bool Channel::hasMembers(Client *client) const
{
	for (std::vector<Client *>::const_iterator it = _members.begin(); it != _members.end(); ++it)
	{
		if (*it == client)
		{
			return true; // If the client is found in the members vector, return true
		}
		return false; // If the client is not found, return false
	}

	// TODO(Alena) : made tmp return true for compiling ( error: non-void function does not return a value in all control paths')
	return true;
}

// Function to check if a client is an operator of the channel
bool Channel::isOperator(Client *client) const
{
	for (std::vector<Client *>::const_iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (*it == client)
			return true; // If the client is found in the operators vector, return true
		return false;	 // If the client is not found, return false
	}

	// TODO(Alena) : made tmp return true for compiling ( error: non-void function does not return a value in all control paths')
	return true;
}
