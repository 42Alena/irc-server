/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/18 13:48:38 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

/*
  handle the PART command: removes the client from a channel.
  sends a PART message to other channel members and the sender.
  sends appropriate error replies if the channel doesn't exist
  or the user is not in the channel.
*/
void handlePart(Server &server, Client &client, const std::vector<std::string> &params)
{
	if (params.empty())
	{
		server.sendToClient(
			client.getFd(),
			replyErr461NeedMoreParams(server.getServerName(), "PART"));
		return;
	}

	std::string channelName = params[0];
	Channel *channel = server.getChannel(channelName);

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
			replyErr441UserNotInChannel(server.getServerName(), client.getNickname(), channelName));
		return;
	}

	// Build and send PART message
	std::string partMsg = ":" + client.getPrefix() + " PART " + channelName + "\r\n";
	channel->sendToChannelExcept(partMsg, client, server);
	server.sendToClient(client.getFd(), partMsg);

	// Remove user from channel
	channel->removeUser(client.getFd(), &client);

	// If channel is now empty, delete it and exit
	if (channel->getMembers().empty())
	{
		server.removeChannel(channel->getName());
		return;
	}

	// Promote a new operator if none remain
	if (channel->getOperators().empty())
	{
		std::map<int, Client *> members = channel->getMembers();
		Client *newOp = NULL;

		for (std::map<int, Client *>::iterator it = members.begin(); it != members.end(); ++it)
		{
			if (it->second)
			{
				newOp = it->second;
				break;
			}
		}

		if (newOp)
		{
			channel->makeOperator(newOp);
			std::string modeMsg = ":ircserv MODE " + channelName + " +o " + newOp->getNickname() + "\r\n";
			channel->sendToChannelAll(modeMsg, server);
		}
	}

	logChannelInfo("[PART] User " + client.getNickname() + " left channel: " + channelName);
}
