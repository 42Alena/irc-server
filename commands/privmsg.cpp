/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 10:36:16 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/14 21:22:30 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

/*
https://www.rfc-editor.org/rfc/rfc1459.html#section-2.3.1

4.4.1 Private messages

      Command: PRIVMSG
   Parameters: <receiver>{,<receiver>} <text to be sent>

   PRIVMSG is used to send private messages between users.  <receiver>
   is the nickname of the receiver of the message.  <receiver> can also
   be a list of names or channels separated with commas.

   The <receiver> parameter may also me a host mask  (#mask)  or  server
   mask  ($mask).   In  both cases the server will only send the PRIVMSG
   to those who have a server or host matching the mask.  The mask  must
   have at  least  1  (one)  "."  in it and no wildcards following the
   last ".".  This requirement exists to prevent people sending messages
   to  "#*"  or "$*",  which  would  broadcast  to  all  users; from
   experience, this is abused more than used responsibly and properly.
   Wildcards are  the  '*' and  '?'   characters.   This  extension  to
   the PRIVMSG command is only available to Operators.

   Numeric Replies:

           ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
           ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
           ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
           ERR_NOSUCHNICK
           RPL_AWAY

   Examples:

:Angel PRIVMSG Wiz :Hello are you receiving this message ?
                                ; Message from Angel to Wiz.

PRIVMSG Angel :yes I'm receiving it !receiving it !'u>(768u+1n) .br ;
                                Message to Angel.

PRIVMSG jto@tolsun.oulu.fi :Hello !
                                ; Message to a client on server



Oikarinen & Reed                                               [Page 32]


RFC 1459              Internet Relay Chat Protocol              May 1993


                                tolsun.oulu.fi with username of "jto".

PRIVMSG $*.fi :Server tolsun.oulu.fi rebooting.
                                ; Message to everyone on a server which
                                has a name matching *.fi.

PRIVMSG #*.edu :NSFNet is undergoing work, expect interruptions
                                ; Message to all users who come from a
                                host which has a name matching *.edu.
*/
void handlePrivateMessage(Server &server, Client &client, const std::vector<std::string> &params)
{

        if (!client.isRegistered())
        {
                server.sendToClient(client.getFd(), replyErr451NotRegistered(server.getServerName(), "PRIVMSG"));
                return;
        }
        if (params.size() == 0) // no recipient
        {
                server.sendToClient(client.getFd(), replyErr411NoRecipient(server.getServerName(), "PRIVMSG"));
                return;
        }
        if (params.size() == 1) // recipient is present, but no message
        {
                server.sendToClient(client.getFd(), replyErr412NoTextToSend(server.getServerName()));
                return;
        }

        const std::string receiver = params[0];
        bool isChannel = (receiver[0] == '#');
        if (isChannel)
        {
                if (!server.isChannelName(receiver))
                {
                        server.sendToClient(client.getFd(), replyErr403NoSuchChannel(server.getServerName(), receiver));
                        return;
                }
                if (!client.isInChannel(receiver))
                {
                        server.sendToClient(client.getFd(), replyErr404CannotSendToChan(server.getServerName(), receiver));
                        return;
                }
        }
        else
        {
                if (!server.isNicknameInUse(receiver))
                {
                        server.sendToClient(client.getFd(), replyErr401NoSuchNick(server.getServerName(), receiver));
                        return;
                }
        }

        // extract message
        std::string message;

        // PRIVMSG must have exactly one recipient before ':'
        size_t colonIndex = 1;
        for (; colonIndex < params.size(); ++colonIndex)
        {
                if (!params[colonIndex].empty() && params[colonIndex][0] == ':')
                {
                        message = params[colonIndex].substr(1); // Remove ':'
                        ++colonIndex;
                        break;
                }
        }

        // if no ':' was found or more than one parameter before message
        if (colonIndex == params.size() || colonIndex > 2)
        {
                server.sendToClient(client.getFd(), replyErr461NeedMoreParams(server.getServerName(), "PRIVMSG"));
                return;
        }

        // join all remaining parts into one clean message string
        for (; colonIndex < params.size(); ++colonIndex)
        {
                message += " " + params[colonIndex];
        }

        // Format for message ":<sender_nick>!<username>@<hostname> PRIVMSG <target> :<message>\r\n"
        std::string messageFormated =
            ":" + client.getNickname() + "!" + client.getUsername() +
            "@" + client.getHost() + " PRIVMSG " + receiver +
            " :" + message + "\r\n";

        if (isChannel)
        {
                Channel *channel = server.getChannel(receiver);
                if (channel)
                        channel->sendToChannelExcept(messageFormated, client); // or *client if pointer
        }

        else
        {
                Client *clientReceiver = server.getClientByNickname(receiver);
                if (clientReceiver)
                        server.sendToClient(clientReceiver->getFd(), messageFormated);
        }
}
