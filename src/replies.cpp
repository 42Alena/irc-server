/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:13:16 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/18 15:24:29 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/replies.hpp"


std::string replyRpl001Welcome(const std::string &server, const std::string &nick, const std::string &user, const std::string &host)
{
    return ":" + server + " " + RPL_WELCOME + " " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host ;
}



std::string replyRpl002YourHost(const std::string &server)
{
    return ":" + server + " " + RPL_YOURHOST + " :Your host is " + server + ", running version 1.0";
}

std::string replyRpl003Created(const std::string &server, const std::string &date)
{
    return ":" + server + " " + RPL_CREATED + " :This server was created " + date ;
}

std::string replyRpl004MyInfo(const std::string &server)
{
    return ":" + server + " " + RPL_MYINFO + " " + server + " 1.0 o oitkl";
}

std::string replyRpl324ChannelMode(const std::string &serverName, const std::string &nickname, const std::string &channel, const std::string &modes)
{

	return ":" + serverName + " 324 " + nickname + " " + channel + " " + modes ;

}


std::string replyRpl331NoTopic(const std::string &server, const std::string &channel, const std::string &topic)
{

    return ":" + server + " " + RPL_NOTOPIC + " " + channel + " :" + topic;

}

std::string replyRpl332Topic(const std::string &server, const std::string &channel, const std::string &topic)
{

    return ":" + server + " " + RPL_TOPIC + " " + channel + " :" + topic ;

}

std::string replyRpl341Inviting(const std::string &server, const std::string &nick, const std::string &channel)
{

    return ":" + server + " 341 " + nick + " " + channel;

}

std::string replyRpl353NamReply(const std::string &server,
                                 const std::string &nick,
                                 const std::string &symbol,
                                 const std::string &channel,
                                 const std::string &names)
{

    return ":" + server + " 353 " + nick + " " + symbol + " " + channel + " :" + names ;

}


std::string replyRpl366EndOfNames(const std::string &server,
                                   const std::string &nick,
                                   const std::string &channel)
{
    return ":" + server + " 366 " + nick + " " + channel + " :End of /NAMES list";
}


std::string replyErr401NoSuchNick(const std::string &server, const std::string &nick)
{
    return ":" + server + " " + ERR_NOSUCHNICK + " " + nick + " :No such nick/channel";
}

std::string replyErr403NoSuchChannel(const std::string &server, const std::string &channel)
{
    return ":" + server + " " + ERR_NOSUCHCHANNEL + " " + channel + " :No such channel";
}

std::string replyErr404CannotSendToChan(const std::string &server, const std::string &channel)
{
    return ":" + server + " " + ERR_CANNOTSENDTOCHAN + " " + channel + " :Cannot send to channel";
}

std::string replyErr411NoRecipient(const std::string &serverName, const std::string &command)
{
    return ":" + serverName + " " ERR_NORECIPIENT " * :No recipient given (" + command + ")";
}

std::string replyErr412NoTextToSend(const std::string &serverName)
{
    return ":" + serverName + " " ERR_NOTEXTTOSEND " * :No text to send";
}

std::string replyErr431NoNickGiven(const std::string &server)
{
    return ":" + server + " " + ERR_NONICKNAMEGIVEN + " :No nickname given";
}

std::string replyErr432ErroneousNick(const std::string &server, const std::string &nick)
{
    return ":" + server + " " + ERR_ERRONEUSNICKNAME + " " + nick + " :Erroneous nickname";
}

std::string replyErr433NickInUse(const std::string &server, const std::string &nick)
{
    return ":" + server + " " + ERR_NICKNAMEINUSE + " " + nick + " :Nickname is already in use";
}

std::string replyErr441UserNotInChannel(const std::string &server, const std::string &nick, const std::string &channel)
{
    return ":" + server + " " + ERR_USERNOTINCHANNEL + " " + nick + " " + channel + " :They aren't on that channel";
}

std::string replyErr442NotOnChannel(const std::string &server, const std::string &channel)
{
    return ":" + server + " " + ERR_NOTONCHANNEL + " " + channel + " :You're not on that channel";
}

std::string replyErr443UserOnChannel(const std::string &server, const std::string &user, const std::string &channel)
{
    return ":" + server + " " + ERR_USERONCHANNEL + " " + user + " " + channel + " :is already on channel";
}

std::string replyErr451NotRegistered(const std::string &server, const std::string &command)
{
    return ":" + server + " 451 " + command + " :You have not registered";
}

std::string replyErr461NeedMoreParams(const std::string &server, const std::string &command)
{
    return ":" + server + " " + ERR_NEEDMOREPARAMS + " " + command + " :Not enough parameters";
}

std::string replyErr462AlreadyRegistered(const std::string &server)
{
    return ":" + server + " " + ERR_ALREADYREGISTRED + " :Unauthorized command (already registered)";
}

std::string replyErr464PasswordMismatch(const std::string &server)
{
    return ":" + server + " " + ERR_PASSWDMISMATCH + " :Password incorrect";
}

std::string replyErr471ChannelIsFull(const std::string &server, const std::string &nick, const std::string &channel)
{
    return ":" + server + " 471 " + nick + " " + channel + " :Cannot join channel (+l)";
}

std::string replyErr472UnknownMode(const std::string &serverName,
                                   const std::string &nickname,
                                   const std::string &modeChar)
{
    return ":" + serverName + " 472 " + nickname + " " + modeChar + " :is unknown mode char to me\r\n";
}

std::string replyErr473InviteOnlyChan(const std::string &server, const std::string &nick, const std::string &channel)
{
    return ":" + server + " 473 " + nick + " " + channel + " :Cannot join channel (+i)";
}


std::string replyErr475BadChannelKey(const std::string &server, const std::string &nick, const std::string &channel)
{
    return ":" + server + " " + ERR_BADCHANNELKEY + " " + nick + " " + channel + " :Cannot join channel (+k)";
}



std::string replyErr476BadChanMask(const std::string &serverName, const std::string &nickname, const std::string &channel)
{
	return ":" + serverName + " 476 " + nickname + " " + channel + " :Bad Channel Mask";
}


std::string replyErr482ChanOpPrivsNeeded(const std::string &server, const std::string &channel)
{
    return ":" + server + " " + ERR_CHANOPRIVSNEEDED + " " + channel + " :You're not channel operator";
}

