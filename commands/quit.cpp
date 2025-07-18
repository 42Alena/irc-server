/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 11:17:52 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/18 13:19:19 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

/*
https://www.rfc-editor.org/rfc/rfc2812.html#page-14
"3.1.7 Quit
	  Command: QUIT
   Parameters: [ <Quit Message> ]
   A client session is terminated with a quit message.
	The server acknowledges this by sending an ERROR message to the client.

   Numeric Replies:
		   None.
   Example:
   QUIT :Gone to have lunch        ; Preferred message format.

   :syrk!kalt@millennium.stealth.net QUIT :Gone to have lunch ; User
								   syrk has quit IRC to have lunch.""
	:<nick>!<user>@<host> QUIT :<message>\r\n
*/
void handleQuit(Server &server, Client &client, const std::vector<std::string> &params)
{
	std::string message = (params.empty()) ? "Client Quit" : params[0];
	if (!message.empty() && message[0] == ':')
		message = message.substr(1);

	std::string quitMsg = ":" + client.getPrefix() + " QUIT :" + message;

	// Copy list of channels before removal
	std::vector<Channel *> channels = client.getChannels();

	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel *channel = *it;
		if (!channel)
			continue;

		// Notify others in the channel about the QUIT
		channel->sendToChannelExcept(quitMsg, client, server);

		// Remove the client from the channel
		channel->removeUser(client.getFd(), &client);

		// If there are members but no operators, promote the first available one
		if (!channel->getMembers().empty() && channel->getOperators().empty())
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
				std::string modeMsg = ":ircserv MODE " + channel->getName() + " +o " + newOp->getNickname();
				channel->sendToChannelAll(modeMsg, server);
			}
		}
	}

	// Send QUIT message to the client itself
	server.sendToClient(client.getFd(), quitMsg);

	// Don't call removeClientFromAllChannels — already handled above
	logServerInfo("[QUIT] " + client.getNickname() + " has quit: " + message);
}
