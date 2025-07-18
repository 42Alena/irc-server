/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:42:32 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/18 08:42:41 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "colors.hpp"
#include <map>
#include <set>

#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/utils.hpp"
#include "../include/commands.hpp"

class Client;
class Server;

class Channel
{
private:
	std::string _name;				  // Name of the channel, must start with '#'
	std::string _topic;				  // Topic of the channel
	std::string _password;			  // Password for the channel, if set
	std::map<int, Client *> _members; // Map to store members of the channel by their file descriptor (fd) ToDo
	std::set<int> _operators;		  // Vector to store operators of the channel ToDo
	std::map<char, bool> _modes;	  // Map to store channel modes (e.g., 'i' for invite-only, 't' for topic settable by operators only)
	std::set<int> _invited;			  // Set of invited users by their file descriptor (fd) ToDo
	std::string _key;				  // Key for the channel, if set
	int _userLimit;					  // Limit on the number of users in the channel

	Channel(const Channel &other);			  // Copy constructor
	Channel &operator=(const Channel &other); // Assignment operator

	// Alena: moved logs to utils.cpp

public:
	// Adding the Channel constructors and destructor
	Channel();											 // Default constructor
	Channel(const std::string &name, Client &createdBy); // Constructor with channel name and channel creator
	~Channel();											 // Destructor

	void addUser(int fd, Client *client);
	void addOperator(int fd);
	void removeUser(int fd, Client *client);

	bool hasMembers(Client *client) const;
	bool isOperator(Client *client) const;

	void sendToChannelAll(const std::string &message, Server &server);
	void sendToChannelExcept(const std::string &message, const Client &clientExcluded, Server &server) const;


	void inviteUser(int fd);	  // Add user to invited list -> done
	bool isInvited(int fd) const; // Check if user is invited -> done
	void removeInvited(int fd);		//Remove after invited uses
	bool isInviteOnly() const;	  // Check if channel is invite-only -> done
	bool isTopicLocked() const;	  // Check if topic is locked for changes -> done
	bool hasKey() const;		  // Check if channel has a key/password set -> done
	bool hasUserLimit() const;	  // Check if channel has a user limit set -> done

	// Setters
	void setTopic(const std::string &topic);
	void setMode(char mode, bool enable);
	void setName(std::string name);
	void setKey(const std::string &key);
	void setLimit(int _userLimit);

	// Getters
	std::string getName() const;
	std::string getNickList() const;
	std::string getTopic() const;
	std::string getPassword() const;
	std::map<int, Client *> getMembers() const;
	std::set<int> getOperators() const;
	std::string getKey() const;
	int getLimit() const;
	std::string getModes() const;
};

#endif
