/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 11:17:52 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/14 17:11:36 by akurmyza         ###   ########.fr       */
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
	std::string message;
	if (params.size() == 0)
	{
		message = "Client Quit";
	}
	else
	{
		message = params[0];
	}
	std::string quitMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHost() + " QUIT :" + message + "\r\n";
	
	std::vector<Channel *> channels = client.getChannels();
	// notify others in each channel
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		(*it)->sendToChannelExcept(quitMsg, client);
	}
	
	server.removeClientFromAllChannels(&client);
}