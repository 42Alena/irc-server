/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lperez-h <lperez-h@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/10 17:38:14 by lperez-h         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/colors.hpp"
#include "../include/utils.hpp"

/* 
 Function to handle a user joining the channel
- it check if the channel is not created and created it if needed
- it check if the user is already a member of the channel
- it check if the channel is invite-only and if the user is invited
- it check if the channel has a key and if the user provided the correct key
- it check if the channel has a user limit and if it is reached
- after all checks, it adds the user to the channel and broadcasts a message to other members
*/
void handleJoin(Server &server, Client &client, const std::vector<std::string> &params)
{
	if(params.empty()){
		std::cout << RED << "Error: No channel name provided." << RST << std::endl;
		return;
	}
	
	std::string channelName = params[0]; // Get the channel name from the parameters
	//luis: should we check if the channel name starts with '#'?
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
	
	if(!channel){
		server.addChannel(channelName, client); 
		std::cout << BLU << "Channel " << channelName << " created by " << client.getNickname() << RST << std::endl;
	}
	
	// Check if the user is already a member of the channel
	if (channel->hasMembers(&client))
	{
		std::cout << RED << "Error: User already in channel." << RST << std::endl;
		return; // If the user is already a member, return with an error message
	}
	
	//Check if the channel is invite-only and if the user is invited
	if (channel->isInviteOnly() && !channel->isInvited(client.getFd()))
	{
		std::cout << RED << "Error: Channel is invite-only." << RST << std::endl;
		return; // If the channel is invite-only and the user is not invited, return with an error message
	}
	
	// Check if the channel has a key and if the user's password matches the channel key
	if (channel->hasKey() && client.getPassword() != channel->getKey())
	{
		std::cout << RED << "Error: Incorrect channel key." << RST << std::endl;
		return; // If the channel has a key and the user's password does not match, return with an error message
	}
	// Check if the channel has a user limit and if it is reached
	if (channel->hasUserLimit() && static_cast<int>(channel->getMembers().size()) >= channel->getLimit())
	{
		std::cout << RED << "Error: Channel user limit reached." << RST << std::endl;
		return; // If the channel has a user limit and it is reached, return with an error message
	}
	channel->addUser(client.getFd(), &client); //add user to channel after all the checks														  // Add the user to the channel
	channel->sendToChannelExcept(client.getNickname() + " has joined the channel.", client); // Notify other members
	std::cout << BLU << "User " << client.getNickname() << " joined channel: " << channelName << RST << std::endl;
}

// Function to handle a user leaving the channel
void handlePart(Server &server, Client &client, const std::vector<std::string> &params)
{
	if(params.empty()){
		std::cout << RED << "Error: No channel name provided." << RST << std::endl;
		return; // If no channel name is provided, return with an error message
	}
	std::string channelName = params[0]; // Get the channel name from the parameters
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the
	if(!channel){
		std::cout << RED << "Error: channel does not exist." << RST << std::endl;
		return;
	}
	if (!channel->hasMembers(&client))
	{
		std::cout << RED << "Error: User not in channel." << RST << std::endl;
		return; // If the user is not a member, return with an error message
	}
	channel->removeUser(client.getFd(), &client); // Remove the user from the channel
	channel->sendToChannelExcept(client.getNickname() + " has left the channel.",  client); // Notify other members
	std::cout << BLU << "User " << client.getNickname() << " left channel: " << channelName << RST << std::endl;
}

/* 
  Function to handle the change of the topic in the channel
- it checks if the user is an operator before allowing them to change the topic
- after this check if set the new topic and broadcasts a message to other members
*/
void handleTopic(Server &server, Client &client, const std::vector<std::string> &params)
{
	
	if(params.size() < 2){
		std::cout << RED << "Error: No topic provided." << RST << std::endl;
		return; // If no topic is provided, return with an error message
	}
	std::string channelName = params[0]; // Get the channel name from the parameters
	std::string newTopic = params[1]; // Get the new topic from the parameters 
	Channel	*channel = server.getChannel(channelName); // Get the channel by name from the server
	if(!channel) {
		std::cout << RED << "Error: Channel does not exist." << RST << std::endl;
		return; // If the channel does not exist, return with an error message
	}
	if (!channel->isOperator(&client))
	{
		std::cout << RED << "Error: Only operators can change the topic." << RST << std::endl;
		return; // If the user is not an operator, return with an error message
	}
	channel->setTopic(newTopic); // Use the setter to change the topic of the channel
	channel->sendToChannelExcept("Topic changed to: " + newTopic, client); // Notify other members
	std::cout << BLU << "Topic changed to: " << newTopic << " by " << client.getNickname() << RST << std::endl;
}

/* 
 Function to handle the change of modes in the channel
- it checks if the user is an operator before allowing them to change modes
- after this check it sets the mode and captures the boolean enable to determine if the mode is being enabled or disabled
- and then broadcasts a message to other members notifying them of the change
*/
void handleMode(Server &server, Client &client, const std::vector<std::string> &params)
{
	if(params.size() < 3){
		std::cout << RED << "Error: Not enough parameters provided." << RST << std::endl;
		return; // If not enough parameters are provided, return with an error message
	}
	std::string channelName = params[0]; // Get the channel name from the parameters
	char mode = params[1][0]; // Get the mode character from the parameters
	bool enable = (params[2] == "true"); //check if the mode is being enabled or disabled
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
	if(!channel) {
		std::cout << RED << "Error: Channel does not exist." << RST << std::endl;
		return;
	}
	if (!channel->isOperator(&client))
	{
		std::cout << RED << "Error: Only operators can change modes." << RST << std::endl;
		return; // If the user is not an operator, return with an error message
	}
	channel->setMode(mode, enable); // Use the setter to change the mode
	std::string modeStatus = enable ? "enabled" : "disabled";
	channel->sendToChannelExcept("Mode '" + std::string(1, mode) + "' has been " + modeStatus + ".", client); // Notify other members
	std::cout << BLU << "Mode '" << mode << "' has been " << modeStatus << " by " << client.getNickname() << RST << std::endl;
}


/* 
 Function to handle inviting a user to the channel
- it checks if the user is an operator before allowing them to invite others
- if the user is already invited, it returns with an error message
- if the user is not invited, it adds them to the invited list and broadcasts a message to other members
*/
void handleInvite(Server &server, Client &client, const std::vector<std::string> &params)
{
	if(params.size() < 2){
		std::cout << RED << "Error: No user fd provided." << RST << std::endl;
		return; // If no user fd is provided, return with an error message
	}
	std::string channelName = params[0]; // Get the channel name from the parameters
	std::stringstream ss(params[1]);
	int targetFd;
	if (!(ss >> targetFd))
	{
		std::cout << RED << "Error: Invalid file descriptor provided." << RST << std::endl;
		return; // Handle invalid input
	}
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
	if (!channel->isOperator(&client))
	{
		std::cout << RED << "Error: Only operators can invite users." << RST << std::endl;
		return; // If the user is not an operator, return with an error message
	}
	if (channel->isInvited(targetFd))
	{
		std::cout << RED << "Error: User already invited." << RST << std::endl;
		return; // If the user is already invited, return with an error message
	}
	channel->inviteUser(targetFd);	// Add the user to the invited list
	channel->sendToChannelExcept(client.getNickname() + " has invited a user to the channel.", client); // Notify other members
	std::cout << BLU << "User with fd " << targetFd << " invited to channel: " << channelName << RST << std::endl;
}

// Function to handle kicking a user from the channel
void handleKick(Server &server, Client &client, const std::vector<std::string> &params)
{
	if(params.size() < 2){
		std::cout << RED << "Error: Not enough params provided." << RST << std::endl;
		return; // If no user fd is provided, return with an error message
	}
	std::string channelName = params[0]; // Get the channel name from the parameters
	std::stringstream ss(params[1]);
	int targetFd;
	if (!(ss >> targetFd))
	{
		std::cout << RED << "Error: Invalid file descriptor provided." << RST << std::endl;
		return; // Handle invalid input
	}
	std::string reason = (params.size() > 2) ? params[2] : "No reason provided"; // Get the reason for the kick, if provided
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
	if(!channel){
		std::cout << RED << "Error: Channel does not exist." << RST << std::endl;
		return; // If the channel does not exist, return with an error message
	}
	if (!channel->isOperator(&client))
	{
		std::cout << RED << "Error: Only operators can kick users." << RST << std::endl;
		return; // If the user is not an operator, return with an error message
	}
	if (!channel->hasMembers(&client))
	{
		std::cout << RED << "Error: User not in channel." << RST << std::endl;
		return; // If the user is not a member, return with an error message
	}
	channel->removeUser(targetFd, &client);																							// Remove the user from the channel
	channel->sendToChannelExcept(client.getNickname() + " has kicked a user from the channel. Reason: " + reason, client); // Notify other members
	std::cout << BLU << "User with fd " << targetFd << " kicked from channel: " << channelName << RST << std::endl;
}

// Function to handle setting a key for the channel
void handleKey(Server &server, Client &client, const std::vector<std::string> &params)
{
	if (params.size() < 2){
		std::cout << RED << "Error: No key provided." << RST << std::endl;
		return; // If no key is provided, return with an error message
	}
	std::string channelName = params[0]; // Get the channel name from the parameters
	std::string key = params[1]; // Get the key from the parameters
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
	if (!channel){
		std::cout << RED << "Error: Channel does not exist." << RST << std::endl;
		return; // If the channel does not exist, return with an error message
	}
	if (!channel->isOperator(&client))
	{
		std::cout << RED << "Error: Only operators can set a key." << RST << std::endl;
		return; // If the user is not an operator, return with an error message
	}
	channel->setKey(key);														  // Use the setter to change the key
	channel->sendToChannelExcept("Channel key has been changed", client); // Notify other members
	std::cout << BLU << "Channel key has been set by " << client.getNickname() << RST << std::endl;
}