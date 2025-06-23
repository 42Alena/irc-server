/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lperez-h <lperez-h@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:42:47 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/23 17:17:16 by lperez-h         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"

//constructor with channel name
Channel::Channel(const std::string& name) : _name(name), _limit(0) {}

//Function to add a user to the channel
void Channel::addUser(Client* client){
	//Add clients to the _members vector
	for(std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it){
		if (*it == client)
			return; // If the client is already a member, do not add again
	}
	_members.push_back(client);
}

//Function to remove a user from the channel
void Channel::removeUser(Client* client){
	//Remove clients from the _members vector
	for(std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it){
		if (*it == client){
			_members.erase(it); // Remove the client from the members vector
			break;
		}
	}
}

//Function to set the topic of the channel
void Channel::setTopic(const std::string& topic) {
	_topic = topic;
}

//Function to set a mode for the channel
//need to define a getter for the modes since is private attribute
void Channel::setMode(char mode, bool enable) {
	_modes[mode] = enable;
}

//Function to set a key for the channel
//need to define a getter for the key since is private attribute
void Channel::setKey(const std::string& key) {
	_key = key;
}

//Function to set a limit for the channel
//need to define a getter for the userlimit since is private attribute
void Channel::setLimit(int limit) {
	_userLimit = limit;
}

//Function to check if a client is a member of the channel
bool Channel::hasMembers(Client* client) const{
	for(std::vector<Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it){
		if (*it == client){
			return true; // If the client is found in the members vector, return true
		}
		return false; // If the client is not found, return false
	}
}

//Function to check if a client is an operator of the channel
bool Channel::isOperator(Client* client) const {
	for(std::vector<Client*>::const_iterator it = _operators.begin(); it != _operators.end(); ++it){
		if (*it == client){
			return true; // If the client is found in the operators vector, return true
		}
		else {
			return false; // If the client is not found, return false
		}
	}
}


