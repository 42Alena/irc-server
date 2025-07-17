/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/17 20:47:15 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

// Function to handle setting a key for the channel
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

	// send standard MODE message to other members
	channel->sendToChannelExcept(
		":" + client.getPrefix() + " MODE " + channelName + " +k :" + key,
		client);

	logChannelInfo("Channel key for " + channelName + " has been set by " + client.getNickname());
}
