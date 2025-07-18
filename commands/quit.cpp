/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 11:17:52 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/18 08:43:45 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

/*
https://www.rfc-editor.org/rfc/rfc2812.html#page-14
"3.1.7 Quit
	  Command: QUIT
   Parameters: [ <Quit Message> ]
   A client session is terminated with a quit message.
	The server acknowledges this by sending an ERROR message to the client.

   Numeric Replies:
		   None.
   Example:
   QUIT :Gone to have lunch        ; Preferred message format.

   :syrk!kalt@millennium.stealth.net QUIT :Gone to have lunch ; User
								   syrk has quit IRC to have lunch.""
	:<nick>!<user>@<host> QUIT :<message>\r\n
*/
void handleQuit(Server &server, Client &client, const std::vector<std::string> &params)
{
	std::string message = (params.empty()) ? "Client Quit" : params[0];

	// build correct QUIT message format for IRC
	std::string quitMsg = ":" + client.getPrefix() + " QUIT :" + message + "\r\n";
	
	// get all channels the client is in
	std::vector<Channel *> channels = client.getChannels();
	

	
	
	// notify others in each channel
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		if (*it)
		{
			
			(*it)->sendToChannelExcept(quitMsg, client, server);
		}
	}
	
	server.sendToClient(client.getFd(), quitMsg); // send back to quitting client
	// remove the client from all those channels:
	server.removeClientFromAllChannels(&client);
	
	logServerInfo("[QUIT] " + client.getNickname() + " has quit: " + message);

}

