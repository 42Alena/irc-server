/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luifer <luifer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:42:32 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/06 11:37:56 by luifer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

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

class Channel
{
private:
	std::string _name;				  // Name of the channel, must start with '#'
	std::string _topic;				  // Topic of the channel
	std::string _password;			  // Password for the channel, if set
	std::map<int, Client*> _members; // Map to store members of the channel by their file descriptor (fd) ToDo
	std::set<int> _operators;		 // Vector to store operators of the channel ToDo
	std::map<char, bool> _modes;	  // Map to store channel modes (e.g., 'i' for invite-only, 't' for topic settable by operators only)
	std::set<int> _invited; // Set of invited users by their file descriptor (fd) ToDo
	std::string _key;				  // Key for the channel, if set
	int _userLimit;					  // Limit on the number of users in the channel

	Channel(const Channel &other); // Copy constructor
	Channel &operator=(const Channel &other); // Assignment operator

public:
	
	//Adding the Channel constructors and destructor
	Channel();// Default constructor
	Channel(const std::string& name, Client createdBy); // Constructor with channel name and channel creator
	~Channel();// Destructor


	void addUser(int fd, Client *client); //done
	void addOperator(int fd); //done
	void removeUser(int fd, Client *client); //done
	
	bool hasMembers(Client *client) const; //done
	bool isOperator(Client *client) const; //done
	void broadCastMessage(const std::string &message, int excludeFd) const; //done
	void inviteUser(int fd); // Add user to invited list -> done
	bool isInvited(int fd) const; // Check if user is invited -> done
	bool isInviteOnly() const; // Check if channel is invite-only -> done
	bool isTopicLocked() const; // Check if topic is locked for changes -> done
	bool hasKey() const; // Check if channel has a key/password set -> done
	bool hasUserLimit() const; // Check if channel has a user limit set -> done

	// Setters
	void setTopic(const std::string &topic); //done 
	void setMode(char mode, bool enable); //done
	void setName(std::string name); //done
	void setKey(const std::string &key); //done
	void setLimit(int _userLimit); //done

	// Getters
	std::string getName() const; // done
	std::string getTopic() const; // -> done
	std::string getPassword() const; // -> done
	std::map<int, Client *> getMembers() const; //-> done
	std::set<int> getOperators() const; // -> done
	std::string getKey() const; // -> done
	int getLimit() const; // -> done
};

#endif

/* 
COPY from todo.txt. You can delete here if it

TODO in Channel:

	[] move some orthodox stuff (copy constructor and operator=) to private  Done!
	   // Real-world IRC channel objects are usually non-copyable — prevents accidental copies  

	REPLACE:  
		[] std::vector<Client*> _members;  
		   => std::map<int, Client*> _members;  
		   // Faster lookup by fd, avoids duplicates, standard for server-side user tracking  

		[] std::vector<Client*> _operators;  
		   => std::set<int> _operators;  
		   // Store only fds of operators, no need to store full Client* here  

		[] removeUser(Client* client)  
		   => void removeUser(int fd);  
		   // Server works with fd — avoid searching by pointer  

		[] void broadCastMessage(const std::string &message, Client *sender) const;  
		   => void broadcast(const std::string &message, int excludeFd) const;  
		   // Simpler to exclude sender by fd, members stored by fd  

	CHANGE:  
		[] Remove either _key or _password — only one field for channel key should exist (suggest: keep _key)  
		   // Subject requires "key" for +k mode — no need for both  

	ADD:  
		[] std::set<int> _invited;  
		   // Store fds of invited clients for invite-only channels (+i)  

		[] void inviteUser(int fd);  
		   // Add client to invited list  

		[] bool isInvited(int fd) const;  
		   // Check if client is invited  

		[] bool _inviteOnly;      // +i — invite-only channel  
		[] bool _topicLocked;     // +t — only operators can change topic  
		[] bool _hasKey;          // +k — key/password is set  
		[] bool _hasUserLimit;    // +l — user limit active  

		[] bool isInviteOnly() const;  
		[] bool isTopicLocked() const;  
		[] bool hasKey() const;  
		[] bool hasUserLimit() const;  
		   // Simple getters to check channel modes — server uses these  



*/