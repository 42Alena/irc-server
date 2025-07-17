/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/18 00:55:53 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

/*
 Function to handle the change of modes in the channel
- It checks if the user is an operator before allowing them to change modes
- Then sets the mode and broadcasts the change
- MODE #channel +i      → enable invite-only
- MODE #channel -t      → disable topic lock
- MODE #channel +k key  → set channel key
- MODE #channel         → query current modes (no change)
*/
void handleMode(Server &server, Client &client, const std::vector<std::string> &params)
{

    if (params.empty())
    {
        server.sendToClient(
            client.getFd(),
            replyErr461NeedMoreParams(server.getServerName(), "MODE"));
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
        server.sendToClient(
            client.getFd(),
            replyErr403NoSuchChannel(server.getServerName(), channelName)); // If the channel does not exist, return with an error message
        return;
    }

    if (!channel->hasMembers(&client))
    {
        server.sendToClient(
            client.getFd(),
            replyErr442NotOnChannel(server.getServerName(), channelName));
        return;
    }

    // Handle mode query: MODE #chan => means "show modes"
    if (params.size() == 1)
    {
        std::string currentModes = channel->getModes(); // example: "+it"
        server.sendToClient(
            client.getFd(),
            replyRpl324ChannelMode(server.getServerName(), client.getNickname(), channelName, currentModes));
        return;
    }


    std::string modeString = params[1]; // e.g. "+i", "-t"

    if ((modeString[0] == '+' || modeString[0] == '-') && !channel->isOperator(&client))
    {
        server.sendToClient(
            client.getFd(),
            replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName));
        return;
    }

    char modeChar = modeString[1];
    bool enable = (modeString[0] == '+');

    channel->setMode(modeChar, enable); // Use the setter to change the mode

    std::string msg = ":" + client.getPrefix() + " MODE " + channelName + " " + modeString;
    channel->sendToChannelAll(msg, server); // Notify all members

    logChannelInfo("Mode '" + modeString + "' applied on " + channelName + " by " + client.getNickname());
}