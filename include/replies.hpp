/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:12:36 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/30 10:14:22 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


#include <string>

// Numeric codes
#define ERR_NONICKNAMEGIVEN   "431"
#define ERR_ERRONEUSNICKNAME  "432"
#define ERR_NICKNAMEINUSE     "433"
#define ERR_NICKCOLLISION     "436"
#define ERR_USERNOTINCHANNEL  "441"
#define ERR_NOTONCHANNEL      "442"
#define ERR_USERONCHANNEL     "443"

// Reply builder functions
std::string replyErrNoNickGiven(const std::string &server);
std::string replyErrErroneusNick(const std::string &server, const std::string &nick);
std::string replyErrNickInUse(const std::string &server, const std::string &nick);
std::string replyErrNickCollision(const std::string &server, const std::string &nick);
std::string replyErrUserNotInChannel(const std::string &server, const std::string &nick, const std::string &channel);
std::string replyErrNotOnChannel(const std::string &server, const std::string &channel);
std::string replyErrUserOnChannel(const std::string &server, const std::string &user, const std::string &channel);


