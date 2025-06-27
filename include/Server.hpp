/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:22 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/27 16:25:33 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <sstream> //intToStr

#include <string>
#include <vector>

#include <cerrno>  //Errors
#include <cstring> //Errors

#include <sys/socket.h> //socket,bind,listen
#include <sys/types.h>	//socket
#include <sys/socket.h>

#include <fcntl.h>		//fcntl
#include <netinet/in.h> //struct sockaddr_in(IPv4)
#include <arpa/inet.h>	//htons(convert hot<=>bytes)
#include <poll.h>		//poll
#include <map>			//std::map https://en.cppreference.com/w/cpp/container/map.html
#include <cstdlib>		//EXIT_SUCCESS, EXIT_FAILURE
#include <algorithm>	//transform()

// from  man poll()
#include <fcntl.h>	   //  open()
#include <poll.h>	   //  poll() and struct pollfd
#include <stdio.h>	   //  std::cout <<(), fstd::cout <<()
#include <stdlib.h>	   //  malloc(), exit()
#include <sys/types.h> //  ssize_t
#include <unistd.h>	   //  read(), close()

#include <sys/socket.h> //accept()
#include <netinet/in.h> // struct sockaddr_in

#include "colors.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Server
{
private:
	//_______________________ Server Configuration _______________________//
	int _port;			   									// Server port
	std::string _password; 									// Password required for connection

	//_______________________ Socket & Poll Management _______________________//
	int _serverFd;											 // Server listening socket (socket())
	std::vector<struct pollfd> _pollFds; 						// Poll file descriptors list

	//_______________________ Client & Channel Tracking _______________________//
	std::map<int, Client *> _clients;							// Map: client fd -> Client*
	std::map<std::string, Channel *> _channels; 				// Map: channel name -> Channel*

	//_______________________ Connection & Client Management _______________________//
	void acceptNewClient();								   // Accept new incoming client
	void removeClient(int fd);							   // Remove client by fd
	void handleClientInput(int fd);						   // Handle received data from client
	void sendToClient(int fd, const std::string &message); // Send message to client

	//_______________________ Command Dispatch & Parsing ___________________________//
	void handleCommand(Client *client, const std::string &line); // Dispatch raw line to specific handler

	//_______________________ Mandatory IRC Command Handlers _______________________//
	void handlePass(Client *client, const std::vector<std::string> &params);		   // PASS command
	void handleNick(Client *client, const std::vector<std::string> &params);		   // NICK command
	void handleUser(Client *client, const std::vector<std::string> &params);		   // USER command
	void handleJoin(Client *client, const std::vector<std::string> &params);		   // JOIN command
	void handlePrivateMessage(Client *client, const std::vector<std::string> &params); // PRIVMSG command

	//_______________check result, error________________
	void checkResult(int result, const std::string &errMsg);
	void logInfo(const std::string &msg);
	void logErrAndThrow(const std::string &msg);

	/*  Later: Needed for full channel logic or bonus commands

	void broadcastToChannel(const std::string& channel, const std::string& msg, int except_fd = -1); // To send a message to all channel members
	void handleKick(Client* client, const std::vector<std::string>& params);
	void handleInvite(Client* client, const std::vector<std::string>& params);
	void handleTopic(Client* client, const std::vector<std::string>& params);
	void handleMode(Client* client, const std::vector<std::string>& params);
*/

public:
	Server();
	Server(int &port, std::string &password);
	Server(const Server &o);
	Server &operator=(const Server &o);
	~Server();

	int run();

	//_________GETTER______________
	int getPort() const;
	std::string getPassword() const;

	//_____HELPER FKT_________
	std::string intToString(int n);
};

std::ostream &operator<<(std::ostream &os, const Server &o);

