/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lperez-h <lperez-h@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:42:47 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/23 14:57:38 by lperez-h         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"

//constructor with channel name
Channel::Channel(const std::string& name) : _name(name), _limit(0) {}

//Function to add a user to the channel
void Channel::addUser(Client* client){
	//Add clients to the _members vector
}

//Function to remove a user from the channel
void Channel::removeUser(Client* client){
	//Remove clients from the _members vector
}

//Function to set the topic of the channel
void Channel::setTopic(const std::string& topic) {_topic = topic;}

//Function to set a mode for the channel
//need to define a getter for the modes since is private attribute
void Channel::setMode(char mode, bool enable) {_modes[mode] = enable;}

//Function to set a key for the channel
//need to define a getter for the key since is private attribute
void Channel::setKey(const std::string& key) {_key = key;}

//Function to set a limit for the channel
//need to define a getter for the userlimit since is private attribute
void Channel::setLimit(int limit) {_userLimit = limit;}

//Function to check if a client is a member of the channel
bool Channel::hasMembers(Client* client) const{
	//return true if _client is in _members vector
}

bool Channel::isOperator(Client* client) const {
	//return true if _client is in _operators vector
}


