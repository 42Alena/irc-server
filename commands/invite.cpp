/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/18 01:49:21 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

/*
 - Checks that the client provides two parameters (nickname and channel).
 - Verifies the channel exists and the inviter is a channel operator.
 - Prevents duplicate invitations.
 - Adds the target user to the channel's invite list.

 Only the inviter and the invited user should see any message.
 Other channel members are NOT notified.
 :ircserv 341 <inviter> <target> <channel>
 IRC INVITE command format:
    INVITE <nickname> <channel>
*/
void handleInvite(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 2)
    {
        server.sendToClient(
            client.getFd(),
            replyErr461NeedMoreParams(server.getServerName(), "INVITE"));
        return;
    }

    std::string targetNick = params[0];        //  <nickname> first
    std::string channelName = params[1];       // <channel> is second
    
  

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
    


    Client *targetClient = server.getClientByNickname(targetNick);
    if (!targetClient)
    {
        server.sendToClient(
            client.getFd(),
            replyErr401NoSuchNick(server.getServerName(), targetNick));  
        return;
    }

    if (!channel->isOperator(&client))
    {
        server.sendToClient(
            client.getFd(),
            replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName));
        return;
    }
    
    if (channel->hasMembers(targetClient))
    {
        server.sendToClient(
            client.getFd(),
            replyErr443UserOnChannel(server.getServerName(), targetNick, channelName));
        return;
    }

     

    
    //Invite the client by fd
    channel->inviteUser(targetClient->getFd());

    // Notify invited user (not required by RFC but useful)
    server.sendToClient(
        targetClient->getFd(),
        ":" + client.getPrefix() + " INVITE " + targetNick + " :" + channelName);

    //  Notify inviter
    server.sendToClient(
        client.getFd(),
        replyRpl341Inviting(server.getServerName(), targetNick, channelName));

    logChannelInfo("[INVITE] " + targetNick + " invited to channel: " + channelName + " by " + client.getNickname());

}


