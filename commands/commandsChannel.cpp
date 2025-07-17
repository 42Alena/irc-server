/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commandsChannel.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/17 18:31:25 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/colors.hpp"
#include "../include/utils.hpp"
#include "../include/replies.hpp"



/*
  Function to handle the change of the topic in the channel
- it checks if the user is an operator before allowing them to change the topic
- after this check if set the new topic and broadcasts a message to other members
*/
void handleTopic(Server &server, Client &client, const std::vector<std::string> &params)
{

	if (params.size() < 2){
		replyErr461NeedMoreParams(server.getServerName(), "TOPIC");
		return;
	}
	std::string channelName = params[0];			   // Get the channel name from the parameters
	std::string newTopic = params[1];				   // Get the new topic from the parameters
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
	if (!channel){
		replyErr403NoSuchChannel(server.getServerName(), channelName);
		return;
	}
	if (!channel->isOperator(&client)){
		replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName);
		return;
	}
	channel->setTopic(newTopic);										   // Use the setter to change the topic of the channel
	channel->sendToChannelExcept("Topic changed to: " + newTopic, client); // Notify other members
	logChannelInfo("Topic changed to: " + newTopic + " by " + client.getNickname());
}

/*
 Function to handle the change of modes in the channel
- it checks if the user is an operator before allowing them to change modes
- after this check it sets the mode and captures the boolean enable to determine if the mode is being enabled or disabled
- and then broadcasts a message to other members notifying them of the change
*/
void handleMode(Server &server, Client &client, const std::vector<std::string> &params)
{
	if (params.size() < 3){
		replyErr461NeedMoreParams(server.getServerName(), "MODE");
		return;
	}
	std::string channelName = params[0];			   // Get the channel name from the parameters
	char mode = params[1][0];						   // Get the mode character from the parameters
	bool enable = (params[2] == "true");			   // check if the mode is being enabled or disabled
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
	if (!channel){
		replyErr403NoSuchChannel(server.getServerName(), channelName); // If the channel does not exist, return with an error message
		return;
	}
	if (!channel->isOperator(&client)){
		replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName);
		return;
	}
	channel->setMode(mode, enable); // Use the setter to change the mode
	std::string modeStatus = enable ? "enabled" : "disabled";
	channel->sendToChannelExcept("Mode '" + std::string(1, mode) + "' has been " + modeStatus + ".", client); // Notify other members
	logChannelInfo("Mode '" + std::string(1, mode) + "' has been " + modeStatus + " by " + client.getNickname());
}

/*
 Function to handle inviting a user to the channel
- it checks if the user is an operator before allowing them to invite others
- if the user is already invited, it returns with an error message
- if the user is not invited, it adds them to the invited list and broadcasts a message to other members
*/
void handleInvite(Server &server, Client &client, const std::vector<std::string> &params)
{
	if (params.size() < 2){
		replyErr461NeedMoreParams(server.getServerName(), "INVITE");
		return;
	}
	std::string channelName = params[0]; // Get the channel name from the parameters
	std::stringstream ss(params[1]);
	int targetFd;
	if (!(ss >> targetFd))
	{
		logChannelError("Error: Invalid file descriptor provided");
		return; // Handle invalid input
	}
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
	if (!channel->isOperator(&client)){
		replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName);
		return;
	}
	if (channel->isInvited(targetFd)){
		replyErr443UserOnChannel(server.getServerName(), client.getNickname(), channelName);			// If the user is already invited, return with an error message
		return;
	}
	channel->inviteUser(targetFd);																		// Add the user to the invited list
	channel->sendToChannelExcept(client.getNickname() + " has invited a user to the channel.", client); // Notify other members
	logChannelInfo("User with fd " + intToString(targetFd) + " invited to channel: " + channelName);
}

// Function to handle kicking a user from the channel
void handleKick(Server &server, Client &client, const std::vector<std::string> &params)
{
	if (params.size() < 2){
		replyErr461NeedMoreParams(server.getServerName(), "KICK");
		return;
	}
	std::string channelName = params[0]; // Get the channel name from the parameters
	std::stringstream ss(params[1]);
	int targetFd;
	if (!(ss >> targetFd))
	{
		logChannelError("Invalid file descriptor provided");
		return; // Handle invalid input
	}
	std::string reason = (params.size() > 2) ? params[2] : "No reason provided"; // Get the reason for the kick, if provided
	Channel *channel = server.getChannel(channelName);							 // Get the channel by name from the server
	if (!channel){
		replyErr403NoSuchChannel(server.getServerName(), channelName); // If the channel does not exist, return with an error message
		return;
	}
	if (!channel->isOperator(&client)){
		replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName);
		return;
	}
	if (!channel->hasMembers(&client)){
		replyErr441UserNotInChannel(server.getServerName(), client.getNickname(), channelName);							   // If the user is not a member, return with an error message
		return;
	}
	channel->removeUser(targetFd, &client);																				   // Remove the user from the channel
	channel->sendToChannelExcept(client.getNickname() + " has kicked a user from the channel. Reason: " + reason, client); // Notify other members
	logChannelInfo("User with fd " + intToString(targetFd) + " kicked from channel: " + channelName);
}

// Function to handle setting a key for the channel
void handleKey(Server &server, Client &client, const std::vector<std::string> &params)
{
	if (params.size() < 2){
		replyErr461NeedMoreParams(server.getServerName(), "KEY");
		return;
	}
	std::string channelName = params[0];			   // Get the channel name from the parameters
	std::string key = params[1];					   // Get the key from the parameters
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
	if (!channel){
		replyErr403NoSuchChannel(server.getServerName(), channelName);
		return;
	}
	if (!channel->isOperator(&client)){
		replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName);
		return;
	}
	channel->setKey(key);												  // Use the setter to change the key
	channel->sendToChannelExcept("Channel key has been changed", client); // Notify other members
	logChannelInfo("Channel key has been set by " + client.getNickname());
}
