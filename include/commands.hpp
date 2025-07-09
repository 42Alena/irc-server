/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 10:36:03 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/09 07:49:16 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


#include <string>
#include <vector>
#include "../include/replies.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"

class Server;
class Client;

//======================== PUBLIC: USER REGISTRATION COMMANDS =====================//

void handlePass(Server &server, Client &client, const std::vector<std::string> &params);
void handleNick(Server &server, Client &client, const std::vector<std::string> &params);
void handleUser(Server &server, Client &client, const std::vector<std::string> &params);
void sendWelcome(Server &server, Client &client);

//======================== PUBLIC: CHANNEL COMMANDS ===============================//

void handleJoin(Server &server, Client &client, const std::vector<std::string> &params);
void handlePart(Server &server, Client &client, const std::vector<std::string> &params);
void handleTopic(Server &server, Client &client, const std::vector<std::string> &params);
void handleMode(Server &server, Client &client, const std::vector<std::string> &params);
void handleInvite(Server &server, Client &client, const std::vector<std::string> &params);
void handleKick(Server &server, Client &client, const std::vector<std::string> &params);

//======================== PUBLIC: PRIVATE MESSAGING ==============================//

void handlePrivateMessage(Server &server, Client &client, const std::vector<std::string> &params);



//======================== PUBLIC: SERVER INTERACTION =============================//

void handlePing(Server &server, Client &client, const std::vector<std::string> &params);
void sendPong(Server &server, Client &client, const std::string &originMessage);
void handleQuit(Server &server, Client &client, const std::vector<std::string> &params);
void sendWelcome(Server &server, Client &client); //registration.cpp

