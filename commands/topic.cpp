/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/17 22:39:32 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

/*
  Function to handle the change or query of the topic in a channel
  - If only channel name is given: sends the current topic or "no topic" message.
  - If topic is to be changed: checks if the user is allowed (based on +t mode).
  - Broadcasts the new topic to all members.
*/
void handleTopic(Server &server, Client &client, const std::vector<std::string> &params)
{

	if (params.empty())
	{
		server.sendToClient(
			client.getFd(),
			replyErr461NeedMoreParams(server.getServerName(), "TOPIC"));
		return;
	}
	std::string channelName = params[0]; // Get the channel name from the parameters

	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server

	if (!channel)
	{
		server.sendToClient(
			client.getFd(),
			replyErr403NoSuchChannel(server.getServerName(), channelName));
		return;
	}

	if (!channel->hasMembers(&client))
	{
		server.sendToClient(
			client.getFd(),
			replyErr442NotOnChannel(server.getServerName(), channelName));
		return;
	}

	// Topic query (view current topic)
	if (params.size() == 1)
	{
		if (channel->getTopic().empty())
		{
			server.sendToClient(
				client.getFd(),
				replyRpl331NoTopic(server.getServerName(), channelName, channel->getTopic()));
		}
		else
		{
			server.sendToClient(
				client.getFd(),
				replyRpl332Topic(server.getServerName(), channelName, channel->getTopic()));
		}
		return;
	}

	std::string newTopic = params[1]; // Get the new topic from the parameters

	if (channel->isTopicLocked() && !channel->isOperator(&client))
	{
		server.sendToClient(
			client.getFd(),
			replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName));
		return;
	}

	channel->setTopic(newTopic); // Use the setter to change the topic of the channel

	// Send proper IRC-formatted topic change message to ALL members
	std::string msg = ":" + client.getPrefix() + " TOPIC " + channelName + " :" + newTopic;
	channel->sendToChannelAll(msg, server);								   // requires sendToChannelAll(msg, server) to be implemented
	
	
	logChannelInfo("Topic changed to: " + newTopic + " by " + client.getNickname());
}
