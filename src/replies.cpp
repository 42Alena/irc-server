/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:13:16 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/30 15:55:34 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/replies.hpp"

//https://www.rfc-editor.org/rfc/rfc1459.html#section-2.4

std::string replyErrNoNickGiven(const std::string &server)
{
	return ":" + server + " " ERR_NONICKNAMEGIVEN " :No nickname given\r\n";
}

std::string replyErrErroneusNick(const std::string &server, const std::string &nick)
{
	return ":" + server + " " ERR_ERRONEUSNICKNAME " " + nick + " :Erroneous nickname\r\n";
}

std::string replyErrNickInUse(const std::string &server, const std::string &nick)
{
	return ":" + server + " " ERR_NICKNAMEINUSE " " + nick + " :Nickname is already in use\r\n";
}

std::string replyErrNickCollision(const std::string &server, const std::string &nick)
{
	return ":" + server + " " ERR_NICKCOLLISION " " + nick + " :Nickname collision KILL\r\n";
}

std::string replyErrUserNotInChannel(const std::string &server, const std::string &nick, const std::string &channel)
{
	return ":" + server + " " ERR_USERNOTINCHANNEL " " + nick + " " + channel + " :They aren't on that channel\r\n";
}

std::string replyErrNotOnChannel(const std::string &server, const std::string &channel)
{
	return ":" + server + " " ERR_NOTONCHANNEL " " + channel + " :You're not on that channel\r\n";
}

std::string replyErrUserOnChannel(const std::string &server, const std::string &user, const std::string &channel)
{
	return ":" + server + " " ERR_USERONCHANNEL " " + user + " " + channel + " :is already on channel\r\n";
}
