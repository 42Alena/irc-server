/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pingPong.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 10:36:19 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/07 18:14:05 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"
/*
https://www.rfc-editor.org/rfc/rfc2812.html#section-3.7.2

PING message

Command: PING
Parameters: <server1> [ <server2> ]

- Used to check if a client or server is still active.
- Servers send PING if no other activity happens for a while.
- If no PONG reply is received, the connection is closed.
- PING can also be sent during active connections.
- When a PING is received, the server must immediately reply with: 
  PONG <server1>
- If <server2> is present, the message gets forwarded there (not needed for ft_irc).

Example:
PING tolsun.oulu.fi
*/
void handlePing(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.empty())
        return;

    const std::string &originMessage = params[0];
    sendPong(server, client, originMessage);
}

/*
https://www.rfc-editor.org/rfc/rfc2812.html#section-3.7.3

PONG message

Command: PONG
- A reply to a PING message.

Example:
PONG csd.bu.edu tolsun.oulu.fi
*/
void sendPong(Server &server, Client &client, const std::string &originMessage)
{
    std::string pongMessage = "PONG " + originMessage + "\r\n";
    server.sendToClient(client.getFd(), pongMessage);
}
