/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lperez-h <lperez-h@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:42:32 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/23 17:24:09 by lperez-h         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <vector>

#define BLUE "\e[1;94m"
#define RED "\e[1;91m"
#define GREEN "\e[1;92m"
#define RESET "\033[0m"

class Channel 
{
	private:
		std::string _name; // Name of the channel, must start with '#'
		std::string _topic; // Topic of the channel
		std::vector<Client*> _members; // Vector to store members of the channel
		std::vector<Client*> _operators; // Vector to store operators of the channel
		std::map<char, bool> _modes; // Map to store channel modes (e.g., 'i' for invite-only, 't' for topic settable by operators only)
		std::string _key; // Key for the channel, if set
		int _userLimit; // Limit on the number of users in the channel

	public:
		Channel();// Default constructor
		Channel(const std::string& name); // Constructor with channel name
		~Channel();// Destructor
		void addUser(Client* client);
		void removeUser(Client* client);
		void setTopic(const std::string& topic);
		void setMode(char mode, bool enable);
		void setKey(const std::string& key);
		void setLimit(int _userLimit);
		bool hasMembers(Client* client) const;
		bool isOperator(Client* client) const;
};

#endif