/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/18 13:34:22 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

/*
  handle the PART command: removes the client from a channel.
  sends a PART message to other channel members and the sender.
  sends appropriate error replies if the channel doesn't exist
  or the user is not in the channel.
*/
void handleQuit(Server &server, Client &client, const std::vector<std::string> &params)
{
	std::string message = (params.empty()) ? "Client Quit" : params[0];
	if (!message.empty() && message[0] == ':')
		message = message.substr(1);

	std::string quitMsg = ":" + client.getPrefix() + " QUIT :" + message + "\r\n";

	// Copy list of channels before removal
	std::vector<Channel *> channels = client.getChannels();

	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel *channel = *it;
		if (!channel)
			continue;

		// Notify others
		channel->sendToChannelExcept(quitMsg, client, server);

		// Remove the client from the channel
		channel->removeUser(client.getFd(), &client);

		// If the channel is now empty, destroy it
		if (channel->getMembers().empty())
		{
			server.removeChannel(channel->getName());
			continue;
		}

		// Promote a new operator if none left
		if (channel->getOperators().empty())
		{
			std::map<int, Client *> members = channel->getMembers();
			Client *newOp = NULL;

			for (std::map<int, Client *>::iterator mIt = members.begin(); mIt != members.end(); ++mIt)
			{
				if (mIt->second)
				{
					newOp = mIt->second;
					break;
				}
			}

			if (newOp)
			{
				channel->makeOperator(newOp);
				std::string modeMsg = ":ircserv MODE " + channel->getName() + " +o " + newOp->getNickname() + "\r\n";
				channel->sendToChannelAll(modeMsg, server);
			}
		}
	}

	// Send QUIT message to the client
	server.sendToClient(client.getFd(), quitMsg);

	logServerInfo("[QUIT] " + client.getNickname() + " has quit: " + message);
}
