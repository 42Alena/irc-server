/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/18 01:11:59 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

/*
 Function: handleKey

 Description:
 Handles the setting of a channel key (MODE +k). This key acts as a password
 required to join the channel.

 Behavior:
 - Only operators are allowed to set the key.
 - Validates channel existence and that the client is in the channel.
 - If valid, sets the channel key using setKey().
 - Broadcasts a standard IRC MODE +k message to all users in the channel.
 - Logs the action for debug or audit purposes.

 Usage:
    KEY #channel secret
    → translates to: MODE #channel +k :secret
*/
void handleKey(Server &server, Client &client, const std::vector<std::string> &params)
{
	if (params.size() < 2)
	{
		server.sendToClient(
			client.getFd(),
			replyErr461NeedMoreParams(server.getServerName(), "KEY")); // send response
		return;
	}

	std::string channelName = params[0]; // Get the channel name from the parameters
	std::string key = params[1];		 // Get the key from the parameters

	// Validate channel name
	if (!server.isChannelName(channelName))
	{
		server.sendToClient(
			client.getFd(),
			replyErr476BadChanMask(server.getServerName(), client.getNickname(), channelName));
		return;
	}

	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server

	if (!channel)
	{
		server.sendToClient(
			client.getFd(),
			replyErr403NoSuchChannel(server.getServerName(), channelName));
		return;
	}

	if (!channel->isOperator(&client))
	{
		server.sendToClient(
			client.getFd(),
			replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName));
		return;
	}

	channel->setKey(key); // Use the setter to change the key

	// send standard MODE message to ALL other members
	channel->sendToChannelAll(
		":" + client.getPrefix() + " MODE " + channelName + " +k :" + key,
		server);

	//KEY #channel secret → MODE #channel +k :secret irc format
	logChannelInfo("Mode '+k' (key) set for " + channelName + " by " + client.getNickname());
}
