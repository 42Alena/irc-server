/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/17 19:13:06 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"



/*
 Function to handle inviting a user to the channel
- it checks if the user is an operator before allowing them to invite others
- if the user is already invited, it returns with an error message
- if the user is not invited, it adds them to the invited list and broadcasts a message to other members
*/
void handleInvite(Server &server, Client &client, const std::vector<std::string> &params)
{
	if (params.size() < 2){
		replyErr461NeedMoreParams(server.getServerName(), "INVITE");
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
	if (!channel->isOperator(&client)){
		replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName);
		return;
	}
	if (channel->isInvited(targetFd)){
		replyErr443UserOnChannel(server.getServerName(), client.getNickname(), channelName);			// If the user is already invited, return with an error message
		return;
	}
	channel->inviteUser(targetFd);																		// Add the user to the invited list
	channel->sendToChannelExcept(client.getNickname() + " has invited a user to the channel.", client); // Notify other members
	logChannelInfo("User with fd " + intToString(targetFd) + " invited to channel: " + channelName);
}
