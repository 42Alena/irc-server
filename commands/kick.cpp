/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/17 19:17:19 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"


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
