/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/17 08:52:51 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"
/*
 Function to handle a user joining the channel
- it check if the channel is not created and created it if needed
- it check if the user is already a member of the channel
- it check if the channel is invite-only and if the user is invited
- it check if the channel has a key and if the user provided the correct key
- it check if the channel has a user limit and if it is reached
- after all checks, it adds the user to the channel and broadcasts a message to other members
*/
void handleJoin(Server &server, Client &client, const std::vector<std::string> &params)
{
    // Reject unregistered clients
    if (!client.isRegistered())
    {
        replyErr451NotRegistered(server.getServerName(), "JOIN");
        return;
    }

    // Missing channel name
    if (params.empty())
    {
        replyErr461NeedMoreParams(server.getServerName(), "JOIN");
        return;
    }

    std::string channelName = params[0]; // Get the channel name from the parameters

    // check channel name
    if (!server.isChannelName(channelName)) // Check for valid prefix, no spaces, length, etc.
    {
        server.sendToClient(
        client.getFd(),
        replyErr476BadChanMask(server.getServerName(), client.getNickname(), channelName));
        return;
    }

    Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
    if (!channel)
    {
        server.addChannel(channelName, client);

        channel = server.getChannel(channelName); // get the just-created channel

        //  first user becomes operator
        channel->addOperator(client.getFd());
    }

    //  check in case if smth goes wrong
    if (!channel)
    {
        logChannelError("JOIN failed: could not get channel " + channelName);
        return;
    }

    // Check if the user is already a member of the channel
    if (channel->hasMembers(&client))
    {
        logChannelInfo("User already in channel");
        return;
    }

    // Check if the channel is invite-only and if the user is invited
    if (channel->isInviteOnly() && !channel->isInvited(client.getFd()))
    {
        replyErr473InviteOnlyChan(server.getServerName(), channelName);
        return;
    }

    // Check if the channel has a key and if the user's password matches the channel key
    if (channel->hasKey() && client.getPassword() != channel->getKey())
    {
        replyErr475BadChannelKey(server.getServerName(), channelName);
        return;
    }
    // Check if the channel has a user limit and if it is reached
    if (channel->hasUserLimit() && static_cast<int>(channel->getMembers().size()) >= channel->getLimit())
    {
        replyErr471ChannelIsFull(server.getServerName(), channelName);
        return;
    }

    channel->addUser(client.getFd(), &client); // add user to channel after all the checks

    client.joinChannel(channel); //  add channel to client's list

    // Add the user to the channel
    channel->sendToChannelExcept(client.getNickname() + " has joined the channel.", client); // Notify other members
    channel->sendToChannelExcept(
        ":" + client.getPrefix() + " JOIN " + channelName,
        client); //  More accurate IRC JOIN message format
    logChannelInfo("User " + client.getNickname() + " joined channel: " + channelName);
}
