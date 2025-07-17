/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:56:02 by lperez-h          #+#    #+#             */
/*   Updated: 2025/07/17 19:06:10 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/colors.hpp"
#include "../include/utils.hpp"
#include "../include/replies.hpp"



/*
 Function to handle the change of modes in the channel
- it checks if the user is an operator before allowing them to change modes
- after this check it sets the mode and captures the boolean enable to determine if the mode is being enabled or disabled
- and then broadcasts a message to other members notifying them of the change
*/
void handleMode(Server &server, Client &client, const std::vector<std::string> &params)
{
	if (params.size() < 3){
		replyErr461NeedMoreParams(server.getServerName(), "MODE");
		return;
	}
	std::string channelName = params[0];			   // Get the channel name from the parameters
	char mode = params[1][0];						   // Get the mode character from the parameters
	bool enable = (params[2] == "true");			   // check if the mode is being enabled or disabled
	Channel *channel = server.getChannel(channelName); // Get the channel by name from the server
	if (!channel){
		replyErr403NoSuchChannel(server.getServerName(), channelName); // If the channel does not exist, return with an error message
		return;
	}
	if (!channel->isOperator(&client)){
		replyErr482ChanOpPrivsNeeded(server.getServerName(), channelName);
		return;
	}
	channel->setMode(mode, enable); // Use the setter to change the mode
	std::string modeStatus = enable ? "enabled" : "disabled";
	channel->sendToChannelExcept("Mode '" + std::string(1, mode) + "' has been " + modeStatus + ".", client); // Notify other members
	logChannelInfo("Mode '" + std::string(1, mode) + "' has been " + modeStatus + " by " + client.getNickname());
}