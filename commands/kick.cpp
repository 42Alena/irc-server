/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/17 21:32:13 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

// Function to handle kicking a user from the channel
void handleKick(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 2)
    {
        server.sendToClient(
            client.getFd(),
            replyErr461NeedMoreParams(server.getServerName(), "KICK"));
        return;
    }

    std::string channelName = params[0]; // Get the channel name from the parameters
    std::string targetNick = params[1];
    std::string reason = (params.size() > 2) ? params[2] : "Kicked";

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
            replyErr403NoSuchChannel(server.getServerName(), channelName)); // If the channel does not exist, return with an error message
        return;
    }

    if (!channel->isOperator(&client))
    {
        server.sendToClient(
            client.getFd(),
            replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName));
        return;
    }

    if (!channel->hasMembers(&client))
    {
        // If the user is not a member, return with an error message
        server.sendToClient(
            client.getFd(),
            replyErr441UserNotInChannel(server.getServerName(), client.getNickname(), channelName));
        return;
    }

    Client *target = server.getClientByNickname(targetNick);

    if (!target)
    {
        server.sendToClient(
            client.getFd(),
            replyErr401NoSuchNick(server.getServerName(), targetNick));
        return;
    }

    if (!channel->hasMembers(target))
    {
        server.sendToClient(
            client.getFd(),
            replyErr441UserNotInChannel(server.getServerName(), targetNick, channelName));
        return;
    }

    // IRC-style KICK message to all channel members
    std::string msg = ":" + client.getPrefix() + " KICK " + channelName + " " + targetNick + " :" + reason;

    channel->removeUser(target->getFd(), target); // Remove the user from the channel.  use the actual target client

    // Notify ALL members + KICK must be  send to kicker too
    channel->sendToChannelAll(msg, server);

    target->leaveChannel(channel);
    
    logChannelInfo("User " + targetNick + " kicked from " + channelName + " by " + client.getNickname());
}
