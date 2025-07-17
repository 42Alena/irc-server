/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/17 18:45:50 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"


/*
  Function to handle the change of the topic in the channel
- it checks if the user is an operator before allowing them to change the topic
- after this check if set the new topic and broadcasts a message to other members
*/
void handleTopic(Server &server, Client &client, const std::vector<std::string> &params)
{

	if (params.size() < 2){
		replyErr461NeedMoreParams(server.getServerName(), "TOPIC");
		return;
	}
	std::string channelName = params[0];			   // Get the channel name from the parameters
	std::string newTopic = params[1];				   // Get the new topic from the parameters
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
	if (!channel){
		replyErr403NoSuchChannel(server.getServerName(), channelName);
		return;
	}
	if (!channel->isOperator(&client)){
		replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName);
		return;
	}
	channel->setTopic(newTopic);										   // Use the setter to change the topic of the channel
	channel->sendToChannelExcept("Topic changed to: " + newTopic, client); // Notify other members
	logChannelInfo("Topic changed to: " + newTopic + " by " + client.getNickname());
}
