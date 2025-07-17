/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commandsChannel.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/17 19:17:07 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/colors.hpp"
#include "../include/utils.hpp"
#include "../include/replies.hpp"

// Function to handle setting a key for the channel
void handleKey(Server &server, Client &client, const std::vector<std::string> &params)
{
	if (params.size() < 2){
		replyErr461NeedMoreParams(server.getServerName(), "KEY");
		return;
	}
	std::string channelName = params[0];			   // Get the channel name from the parameters
	std::string key = params[1];					   // Get the key from the parameters
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
	if (!channel){
		replyErr403NoSuchChannel(server.getServerName(), channelName);
		return;
	}
	if (!channel->isOperator(&client)){
		replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName);
		return;
	}
	channel->setKey(key);												  // Use the setter to change the key
	channel->sendToChannelExcept("Channel key has been changed", client); // Notify other members
	logChannelInfo("Channel key has been set by " + client.getNickname());
}
