/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/18 01:26:45 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

/*
 Handle the INVITE command.

 - Checks that the client provides two parameters (channel name and target fd).
 - Verifies the channel exists and the inviter is a channel operator.
 - Prevents duplicate invitations.
 - Adds the target user to the channel's invite list.
 - Notifies all other channel members.
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

    std::string channelName = params[0]; // Get the channel name from the parameters
 
    std::stringstream ss(params[1]);
  
    int targetFd;
 
    if (!(ss >> targetFd))
    {
        logChannelError("Error: Invalid file descriptor provided");
        return; // Handle invalid input
    }
  
    Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
   
    if (!channel->isOperator(&client))
    {
        
        server.sendToClient(
            client.getFd(),
        replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName));
        return;
    }
    
    if (channel->isInvited(targetFd))
    {
        
        server.sendToClient(
            client.getFd(),
        replyErr443UserOnChannel(server.getServerName(), client.getNickname(), channelName)); // If the user is already invited, return with an error message
        return;
    }
    
    channel->inviteUser(targetFd);                                                                      // Add the user to the invited list
    
    channel->sendToChannelExcept(client.getNickname() + " has invited a user to the channel.", client); // Notify other members
    
    logChannelInfo("User with fd " + intToString(targetFd) + " invited to channel: " + channelName);
}
