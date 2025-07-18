/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/18 15:39:19 by akurmyza         ###   ########.fr       */
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

    const std::string &channelName = params[0];

    if (!server.isChannelName(channelName))
    {
        server.sendToClient(
            client.getFd(),
            replyErr476BadChanMask(server.getServerName(), client.getNickname(), channelName));
        return;
    }

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
            replyErr442NotOnChannel(server.getServerName(), channelName));
        return;
    }

    // MODE #chan (no change, just show current modes)
    if (params.size() == 1)
    {
        std::string currentModes = channel->getModes();
        server.sendToClient(
            client.getFd(),
            replyRpl324ChannelMode(server.getServerName(), client.getNickname(), channelName, currentModes));
        return;
    }

    const std::string &modeString = params[1];
    if (modeString.empty() || (modeString[0] != '+' && modeString[0] != '-'))
    {
        server.sendToClient(
            client.getFd(),
            replyErr472UnknownMode(server.getServerName(), client.getNickname(), modeString));
        return;
    }

    if (!channel->isOperator(&client))
    {
        server.sendToClient(
            client.getFd(),
            replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName));
        return;
    }

    bool enable = (modeString[0] == '+');
    size_t argIndex = 2;
    std::string appliedModes;
    std::vector<std::string> modeArgs;

    for (size_t i = 1; i < modeString.size(); ++i)
    {
        char modeChar = modeString[i];

        if (std::string("itkol").find(modeChar) == std::string::npos)
        {
            server.sendToClient(
                client.getFd(),
                replyErr472UnknownMode(server.getServerName(), client.getNickname(), std::string(1, modeChar)));
            continue;
        }

        switch (modeChar)
        {
            case 'i':
            case 't':
                channel->setMode(modeChar, enable);
                break;

            case 'k':
                if (enable)
                {
                    if (argIndex >= params.size())
                    {
                        server.sendToClient(
                            client.getFd(),
                            replyErr461NeedMoreParams(server.getServerName(), "MODE"));
                        continue;
                    }
                    channel->setKey(params[argIndex]);
                    channel->setMode('k', true);
                    modeArgs.push_back(params[argIndex]);
                    ++argIndex;
                }
                else
                {
                    channel->setKey("");
                    channel->setMode('k', false);
                }
                break;

            case 'l':
                if (enable)
                {
                    if (argIndex >= params.size())
                    {
                        server.sendToClient(
                            client.getFd(),
                            replyErr461NeedMoreParams(server.getServerName(), "MODE"));
                        continue;
                    }
                    int limit = std::atoi(params[argIndex].c_str());
                    channel->setUserLimit(limit);
                    channel->setMode('l', true);
                    modeArgs.push_back(params[argIndex]);
                    ++argIndex;
                }
                else
                {
                    channel->setUserLimit(-1);
                    channel->setMode('l', false);
                }
                break;

            case 'o':
                if (argIndex >= params.size())
                {
                    server.sendToClient(
                        client.getFd(),
                        replyErr461NeedMoreParams(server.getServerName(), "MODE"));
                    continue;
                }
                {
                    Client *target = server.getClientByNickname(params[argIndex]);
                    if (!target || !channel->hasMembers(target))
                    {
                        server.sendToClient(
                            client.getFd(),
                            replyErr441UserNotInChannel(server.getServerName(), params[argIndex], channelName));
                        ++argIndex;
                        continue;
                    }

                    if (enable)
                        channel->addOperator(target);
                    else
                        channel->removeOperator(target);

                    modeArgs.push_back(params[argIndex]);
                    ++argIndex;
                }
                break;
        }

        if (appliedModes.empty())
            appliedModes += (enable ? "+" : "-");
        appliedModes += modeChar;
    }

    if (!appliedModes.empty())
    {
        std::string msg = ":" + client.getPrefix() + " MODE " + channelName + " " + appliedModes;
        for (size_t i = 0; i < modeArgs.size(); ++i)
            msg += " " + modeArgs[i];
        channel->sendToChannelAll(msg, server);

        logChannelInfo("Mode '" + appliedModes + "' applied on " + channelName + " by " + client.getNickname());
    }
}
