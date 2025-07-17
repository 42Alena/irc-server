/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   registration.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 11:15:08 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/17 11:21:28 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/commands.hpp"

// Contains:
// handlePass
// handleNick
// handleUser
// sendWelcome

/*
https://www.rfc-editor.org/rfc/rfc2812.html#section-3.1.2
Password message

	Command: PASS
   Parameters: <password>

   The PASS command is used to set a 'connection password'.  The
   optional password can and MUST be set before any attempt to register
   the connection is made.  Currently this requires that user send a
   PASS command before sending the NICK/USER combination.

   Numeric Replies:

	   ERR_NEEDMOREPARAMS                        ERR_ALREADYREGISTRED
		replyErr461NeedMoreParams()				replyErr462AlreadyRegistered()

		ERR_PASSWDMISMATCH
		replyErr464PasswordMismatch
   Example:

		   PASS secretpasswordhere

Typical raw IRC message from client:
PASS mysecretpassword
*/
void handlePass(Server &server, Client &client, const std::vector<std::string> &params)
{
	if (params.empty())
	{
		server.sendToClient(client.getFd(), replyErr461NeedMoreParams(server.getServerName(), "PASS"));
		return;
	}

	if (!isValidPassword(params[0]))
	{
		server.sendToClient(client.getFd(), replyErr464PasswordMismatch(server.getServerName()));
		return;
	}

	client.setHasProvidedPass(true);

	if (client.isRegistered())
	{
		sendWelcome(server, client);
	}
}

/*
https://www.rfc-editor.org/rfc/rfc1459.html#section-2.3.1
   <user>       ::= <nonwhite> { <nonwhite> }
   <nonwhite>   ::= <any 8bit code except SPACE (0x20), NUL (0x0), CR
					 (0xd), and LF (0xa)
3.1.3 User message
 Command: USER
Parameters: <user> <mode> <unused> <realname>
=>	must have at least 4 parameters. Else send ERR_NEEDMOREPARAMS

- <username>: non-space, non-control characters (max 9 chars recommended)
 - <mode>: numeric bitmask (bit 2 = 'w' mode, bit 3 = 'i' mode)
 - <unused>  => ignore it present for historical reasons, but is ignored by modern servers.
			It must be present in the command for correct syntax, but its value doesn't matter.
- <realname>: can contain spaces, starts after ':'

Numeric Replies:
		ERR_NEEDMOREPARAMS      replyErr461NeedMoreParams
		ERR_ALREADYREGISTRED	replyErr462AlreadyRegistered
Example:
   USER guest 0 * :Ronnie Reagan   ; User registering themselves with a
								   username of "guest" and real name
								   "Ronnie Reagan".

   USER guest 8 * :Ronnie Reagan   ; User registering themselves with a
								   username of "guest" and real name
								   "Ronnie Reagan", and asking to be set
								   invisible.


https://www.rfc-editor.org/rfc/rfc2812.html#section-1.2.1
Users
	user       =  1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF )
				  ; any octet except NUL, CR, LF, " " and "@"

https://www.rfc-editor.org/rfc/rfc2812.html#section-1.2.1
The <mode> parameter should be a numeric, and can be used to
   automatically set user modes when registering with the server.  This
   parameter is a bitmask, with only 2 bits having any signification: if
   the bit 2 is set, the user mode 'w' will be set and if the bit 3 is
   set, the user mode 'i' will be set.  (See Section 3.1.5 "User
   Modes").
   https://www.rfc-editor.org/rfc/rfc2812.html#section-3.1.5
<mode> Value	Meaning
4	'w' set (WHOIS visible)
8	'i' set (invisible)
*/
void handleUser(Server &server, Client &client, const std::vector<std::string> &params)
{
	// Parameters: <user> <mode> <unused> <realname>
	if (params.size() < 4)
	{
		server.sendToClient(client.getFd(), replyErr461NeedMoreParams(server.getServerName(), "USER"));
		return;
	}

	const std::string &username = params[0];
	const std::string &modeStr = params[1];
	// const std::string &unused = params[2]; // must ignore, only historical

	// join all parts of realname in case it's split across params
	std::string realname;
	for (size_t i = 3; i < params.size(); ++i)
	{
		if (!realname.empty())
			realname += " ";
		realname += params[i];
	}

	if (client.isRegistered())
	{
		server.sendToClient(client.getFd(), replyErr462AlreadyRegistered(server.getServerName()));
		return;
	}

	if (!isValidUsername(username))
	{
		server.sendToClient(client.getFd(), replyErr461NeedMoreParams(server.getServerName(), "USER"));
		return;
	}
	client.setUsername(username);

	int userMode = std::atoi(modeStr.c_str());
	if (userMode & 4) // bit 2 = 'w' = (WHOIS visible)
		client.addUserMode('w');
	if (userMode & 8) // bit 3 =  'i' = (invisible)
		client.addUserMode('i');

	// <realname> starts after : and can contain spaces
	if (realname.empty() || realname[0] != ':' || realname.substr(1).empty())
	{
		server.sendToClient(client.getFd(), replyErr461NeedMoreParams(server.getServerName(), "USER"));
		return;
	}
	client.setRealname(realname.substr(1));

	client.setHasProvidedUser(true);

	if (client.isRegistered())
	{
		sendWelcome(server, client);
	}
}

/*
	https://www.rfc-editor.org/rfc/rfc2812.html#section-3.1.2
"
Nick message
	Command: NICK
   Parameters: <nickname>
   NICK command is used to give user a nickname or change the existingone.

   Numeric Replies:

		   ERR_NONICKNAMEGIVEN
		   ERR_NICKNAMEINUSE
		   ERR_ERRONEUSNICKNAME

   Examples:
   NICK Wiz                ; Introducing new nick "Wiz" if session is
						   still unregistered, or user changing his
						   nickname to "Wiz"

   :WiZ!jto@tolsun.oulu.fi NICK Kilroy
						   ; Server telling that WiZ changed his
						   nickname to Kilroy.
"
 */
void handleNick(Server &server, Client &client, const std::vector<std::string> &params)
{
	if (params.empty())
	{
		server.sendToClient(client.getFd(), replyErr431NoNickGiven(server.getServerName()));
		return;
	}

	const std::string newNickname = params[0];

	// block if PASS not sent
	if (!client.getHasProvidedPass())
	{
		server.sendToClient(client.getFd(), replyErr451NotRegistered(server.getServerName(), "NICK"));
		return;
	}

	if (server.isNicknameInUse(newNickname))
	{
		server.sendToClient(client.getFd(), replyErr433NickInUse(server.getServerName(), newNickname));
		return;
	}

	if (!isValidNickname(newNickname))
	{
		server.sendToClient(client.getFd(), replyErr432ErroneousNick(server.getServerName(), newNickname));
		return;
	}

	const std::string oldNickname = client.getNickname();
	client.setNickname(newNickname);
	client.setHasProvidedNick(true);

	// Broadcast nick change to client and all channels they are in
	if (!oldNickname.empty())
	{
		std::string messageNickChange = ":" + oldNickname + " NICK " + newNickname + "\r\n";

		// Send message to the client
		server.sendToClient(client.getFd(), messageNickChange);

		// Send message to all clients in the same channels
		const std::vector<Channel *> &clientChannels = client.getChannels();
		for (std::vector<Channel *>::const_iterator it = clientChannels.begin(); it != clientChannels.end(); ++it)
			(*it)->sendToChannelExcept(messageNickChange, client);
	}

	client.setHasProvidedNick(true);

	if (client.isRegistered())
	{
		sendWelcome(server, client);
	}
}

/*
https://www.rfc-editor.org/rfc/rfc2812.html#section-3.1.5
3.1 Connection Registration

   The commands described here are used to register a connection with an
   IRC server as a user as well as to correctly disconnect.

   A "PASS" command is not required for a client connection to be registered,
   but it MUST precede the latter of the NICK/USER
   combination (for a user connection) or the SERVICE command (for a
   service connection). The RECOMMENDED order for a client to register
   is as follows:

						   1. Pass message
		   2. Nick message                 2. Service message
		   3. User message

   Upon success, the client will receive an RPL_WELCOME (for users) or
   RPL_YOURESERVICE (for services) message indicating that the
   connection is now registered and known the to the entire IRC network.
   The reply message MUST contain the full client identifier upon which
   it was registered.
																	_host
   :serverName 001 nick :Welcome to the IRC network nick!username@127.0.0.1
																@localhost
*/
void sendWelcome(Server &server, Client &client)
{
	logServerInfo("Welcome message sent to client '" + client.getNickname() + "' (fd=" + intToString(client.getFd()) + ", ip=" + client.getHost() + ")");

	server.sendToClient(client.getFd(),
						replyRpl001Welcome(server.getServerName(), client.getNickname(), client.getUsername(), client.getHost()));
}