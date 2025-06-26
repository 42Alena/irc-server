/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:22 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/26 13:03:38 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>

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
	int _port;
	std::string _password;
	int _serverFd; // sserver socket()
	std::vector<struct pollfd> _pollFds;

	std::map<int, Client*> _clients;
	//adding the channels map to manage channels from server
	std::map<std::string, Channel*> _channels;

	static void throwRuntimeErr(const std::string& errMsg);

	//Declaring some methods that the Server class will use internally
	void acceptNewClient(); //To accept new clients
	void removeClient(int fd); //to remove a client from the server
	void handleClientInput(int fd); //to handle input from a client
	void sendToClient(int fd, const std::string& message); //to send a message to a specific client
	void broadcastToChannel(const std::string& channel, const std::string& msg, int except_fd = -1);// to send a message to all the channel members

	//Handle commands from the clients: these are the methods that will handle the commands sent by the clients
	// These methods will be called from handleCommand() method
	// They will parse the parameters and execute the command
	// they will also send the response to the client
	// and broadcast the response to the channel members if needed
	void handleCommand(Client* client, const std::string& line);
	void handlePass(Client* client, const std::vector<std::string>& params);
	void handleNick(Client* client, const std::vector<std::string>& params);
	void handleUser(Client* client, const std::vector<std::string>& params);
	void handleJoin(Client* client, const std::vector<std::string>& params);
	void handlePrivateMessage(Client* client, const std::vector<std::string>& params);
	void handleKick(Client* client, const std::vector<std::string>& params);
	void handleInvite(Client* client, const std::vector<std::string>& params);
	void handleTopic(Client* client, const std::vector<std::string>& params);
	void handleMode(Client* client, const std::vector<std::string>& params);
	
	void checkResult(int result, const std::string &errMsg);

	void logInfo(const std::string &msg);
	void logErrAndThrow(const std::string &msg);

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
};
std::ostream &operator<<(std::ostream &os, const Server &o);