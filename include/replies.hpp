/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:12:36 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/09 00:05:41 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>

// =============================================================
//                         Replies.hpp
//     Full IRC Replies with Original RFC Comments - ft_irc
// 5.1 Command responses
 //  Numerics   001 to 099 are used for client-server
 //  connections only and should never travel between servers.
//     200-399 Replies  generated in the response to commands
// https://www.rfc-editor.org/rfc/rfc1459.html#section-2.4
// =============================================================


// =============================================================
//                         Numeric Codes
// =============================================================

#define RPL_WELCOME       "001"
#define RPL_YOURHOST      "002"
#define RPL_CREATED       "003"
#define RPL_MYINFO        "004"
#define RPL_TOPIC         "332"
#define RPL_NAMREPLY      "353"
#define RPL_ENDOFNAMES    "366"

#define ERR_NOSUCHNICK        "401"
#define ERR_NOSUCHCHANNEL     "403"
#define ERR_CANNOTSENDTOCHAN  "404"
#define ERR_NONICKNAMEGIVEN   "431"
#define ERR_ERRONEUSNICKNAME  "432"
#define ERR_NICKNAMEINUSE     "433"
#define ERR_USERNOTINCHANNEL  "441"
#define ERR_NOTONCHANNEL      "442"
#define ERR_USERONCHANNEL     "443"
#define ERR_NOTREGISTERED     "451"
#define ERR_NEEDMOREPARAMS    "461"
#define ERR_ALREADYREGISTRED  "462"
#define ERR_PASSWDMISMATCH    "464"
#define ERR_CHANNELISFULL     "471"
#define ERR_INVITEONLYCHAN    "473"
#define ERR_BADCHANNELKEY     "475"
#define ERR_CHANOPRIVSNEEDED  "482"

// =============================================================
//                     Reply Builders with RFC Comments
// =============================================================

/* 001 RPL_WELCOME
   "Welcome to the Internet Relay Network <nick>!<user>@<host>"
   - Used to welcome a client once registration is complete.
*/
std::string replyRpl001Welcome(const std::string &server, const std::string &nick, const std::string &user, const std::string &host);

/* 002 RPL_YOURHOST
   "Your host is <servername>, running version <version>"
   - Displays the server name and version.
*/
std::string replyRpl002YourHost(const std::string &server);

/* 003 RPL_CREATED
   "This server was created <date>"
   - Server creation date information.
*/
std::string replyRpl003Created(const std::string &server, const std::string &date);

/* 004 RPL_MYINFO
   "<servername> <version> <available user modes> <available channel modes>"
   - Provides server information, including version and supported modes.
*/
std::string replyRpl004MyInfo(const std::string &server);

/* 332 RPL_TOPIC
   "<channel> :<topic>"
   - Displays the topic for a channel.
*/
std::string replyRpl332Topic(const std::string &server, const std::string &channel, const std::string &topic);

/* 353 RPL_NAMREPLY
   "= <channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
   - Lists users visible on the channel.
*/
std::string replyRpl353NamReply(const std::string &server, const std::string &channel, const std::string &names);

/* 366 RPL_ENDOFNAMES
   "<channel> :End of /NAMES list"
   - Indicates the end of the NAMES list for a channel.
*/
std::string replyRpl366EndOfNames(const std::string &server, const std::string &channel);

/* 401 ERR_NOSUCHNICK
   "<nickname> :No such nick/channel"
   - Used when the given nickname does not exist.
*/
std::string replyErr401NoSuchNick(const std::string &server, const std::string &nick);

/* 403 ERR_NOSUCHCHANNEL
   "<channel> :No such channel"
   - Returned when the client tries to access a nonexistent channel.
*/
std::string replyErr403NoSuchChannel(const std::string &server, const std::string &channel);

/* 404 ERR_CANNOTSENDTOCHAN
   "<channel> :Cannot send to channel"
   - Sent when a message cannot be delivered to the specified channel.
*/
std::string replyErr404CannotSendToChan(const std::string &server, const std::string &channel);

/* 411 ERR_NORECIPIENT
   ":No recipient given (<command>)"
   - Returned when no recipient is given with a command like PRIVMSG.
*/
std::string replyErr411NoRecipient(const std::string &server, const std::string &command)
{
    return ":" + server + " 411 * :No recipient given (" + command + ")\r\n";
}

/* 412 ERR_NOTEXTTOSEND
   ":No text to send"
   - Returned when no text is provided with a PRIVMSG or NOTICE.
*/
std::string replyErr412NoTextToSend(const std::string &server)
{
    return ":" + server + " 412 * :No text to send\r\n";
}

/* 431 ERR_NONICKNAMEGIVEN
   ":No nickname given"
   - Returned when a nickname parameter is expected but missing.
*/
std::string replyErr431NoNickGiven(const std::string &server);

/* 432 ERR_ERRONEUSNICKNAME
   "<nick> :Erroneous nickname"
   - Returned when a nickname contains invalid characters.
*/
std::string replyErr432ErroneousNick(const std::string &server, const std::string &nick);

/* 433 ERR_NICKNAMEINUSE
   "<nick> :Nickname is already in use"
   - Sent when a nickname is already taken.
*/
std::string replyErr433NickInUse(const std::string &server, const std::string &nick);

/* 441 ERR_USERNOTINCHANNEL
   "<nick> <channel> :They aren't on that channel"
   - Used when a specified user is not on the target channel.
*/
std::string replyErr441UserNotInChannel(const std::string &server, const std::string &nick, const std::string &channel);

/* 442 ERR_NOTONCHANNEL
   "<channel> :You're not on that channel"
   - Returned when a client tries to act on a channel they aren't part of.
*/
std::string replyErr442NotOnChannel(const std::string &server, const std::string &channel);

/* 443 ERR_USERONCHANNEL
   "<user> <channel> :is already on channel"
   - Sent when a user is already present on the channel.
*/
std::string replyErr443UserOnChannel(const std::string &server, const std::string &user, const std::string &channel);

/* 451 ERR_NOTREGISTERED
   ":You have not registered"
   - Sent when a command is issued before completing registration.
*/
std::string replyErr451NotRegistered(const std::string &server);

/* 461 ERR_NEEDMOREPARAMS
   "<command> :Not enough parameters"
   - Returned when a command lacks required parameters.
*/
std::string replyErr461NeedMoreParams(const std::string &server, const std::string &command);

/* 462 ERR_ALREADYREGISTRED
   ":Unauthorized command (already registered)"
   - Returned by the server to any link which tries to
    change part of the registered details (such as
    password or user details from second USER message).
*/
std::string replyErr462AlreadyRegistered(const std::string &server);

/* 464 ERR_PASSWDMISMATCH
   ":Password incorrect"
   - Used when the provided password is incorrect.
*/
std::string replyErr464PasswordMismatch(const std::string &server);

/* 471 ERR_CHANNELISFULL
   "<channel> :Cannot join channel (+l)"
   - Sent when a channel has reached its user limit.
*/
std::string replyErr471ChannelIsFull(const std::string &server, const std::string &channel);

/* 473 ERR_INVITEONLYCHAN
   "<channel> :Cannot join channel (+i)"
   - Returned when trying to join an invite-only channel without an invitation.
*/
std::string replyErr473InviteOnlyChan(const std::string &server, const std::string &channel);

/* 475 ERR_BADCHANNELKEY
   "<channel> :Cannot join channel (+k)"
   - Used when an incorrect channel key (password) is provided.
*/
std::string replyErr475BadChannelKey(const std::string &server, const std::string &channel);

/* 482 ERR_CHANOPRIVSNEEDED
   "<channel> :You're not channel operator"
   - Sent when a privileged command is issued by a non-operator.
*/
std::string replyErr482ChanOpPrivsNeeded(const std::string &server, const std::string &channel);

