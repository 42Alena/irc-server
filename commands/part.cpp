/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/17 16:23:50 by akurmyza         ###   ########.fr       */
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

    std::string partMsg = ":" + client.getPrefix() + " PART " + channelName;

    channel->sendToChannelExcept(partMsg, client);
    server.sendToClient(client.getFd(), partMsg);

    channel->removeUser(client.getFd(), &client);

    logChannelInfo("[PART] User " + client.getNickname() + " left channel: " + channelName);
}
