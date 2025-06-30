/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lperez-h <lperez-h@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:42:32 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/30 16:46:58 by lperez-h         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "colors.hpp" /// Alena: replaced hardcoded color defines with colors.hpp
#include <map>		  // Alena: added for map

#include "../include/Server.hpp"
#include "../include/Client.hpp"

// TODO (Alena): added forward declaration Client class before use it
class Client;

class Channel
{
private:
	std::string _name;				  // Name of the channel, must start with '#'
	std::string _topic;				  // Topic of the channel
	std::vector<Client *> _members;	  // Vector to store members of the channel
	std::vector<Client *> _operators; // Vector to store operators of the channel
	std::map<char, bool> _modes;	  // Map to store channel modes (e.g., 'i' for invite-only, 't' for topic settable by operators only)
	std::string _key;				  // Key for the channel, if set
	int _userLimit;					  // Limit on the number of users in the channel

public:
	// TODO (Alena): Function definition for 'Channel' not found.
	//Adding the Channel constructors and destructor
	Channel();// Default constructor
	Channel(const std::string& name); // Constructor with channel name
	Channel(const Channel &other); // Copy constructor
	Channel &operator=(const Channel &other); // Assignment operator
	~Channel();// Destructor
	
	// TODO (Alena): commented out, _limit is not declared in Channel.cpp, causes error C/C++(292)

	void addUser(Client *client); //done
	void removeUser(Client *client); //done
	void setTopic(const std::string &topic);
	void setMode(char mode, bool enable);
	void setName(std::string name);
	void setKey(const std::string &key);
	void setLimit(int _userLimit);
	bool hasMembers(Client *client) const;
	bool isOperator(Client *client) const;

	// Getters
	std::string getName() const;
	std::string getTopic() const;
	std::string getKey() const;
	int getLimit() const;
	
	
};

#endif