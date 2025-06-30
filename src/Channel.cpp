/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lperez-h <lperez-h@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:42:47 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/30 16:55:56 by lperez-h         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/colors.hpp"

// TODO (Alena):: commented out, "_limit" is not a nonstatic data member or base class of class "Channel"C/C++(292)
// constructor with channel name
// Channel::Channel(const std::string& name) : _name(name), _limit(0) {}

// Function to add a clients to the _members vector inside the channel
void Channel::addUser(Client *client) {	_members.push_back(client); }

// This function checks if the user is an operator before removing them
// If the user is an operator, it prints an error message and does not remove them
// If the user is not an operator, it searches for the user in the _members vector and removes them if found
void Channel::removeUser(Client *client)
{
	if (isOperator(client))
	{
		std::cout << RED << "Error: Cannot remove operator from channel." << RST << std::endl;
		return;
	}
	for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end(); ++it){
		if(*it == client){
			_members.erase(it); // Erase the client from the _members vector if found
			std::cout << RED << "Client removed from channel: " << RST << std::endl;
			return;
		}
	}
	std::cout << BLU << "Client not found in this channel, are you sure is here?." << RST << std::endl;
}

//======================== SETTERS ===================================//
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
void Channel::setLimit(int limit) {
	 
	_userLimit = limit; 
}

//======================== GETTERS ===================================//

int Channel::getLimit() const {
	return _userLimit; // Return the user limit of the channel
}



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
