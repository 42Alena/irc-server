/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lperez-h <lperez-h@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:16 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/23 14:47:34 by lperez-h         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Server.hpp"
#pragma once

#include <iostream>

#include <iomanip>
#include <iostream>
#include <string>

class Client
{
private:
	int _fd;				   // read,write,clients in poll,close(fd). -1=nosocket
	std::string _nickName;	   // NICK, private msg
	std::string _receivedData; // temp stores data from the client (accumulates with each recv() until \r\n).
	// for LATER...
	//  std::string _username;  //USER, private msg
	// bool _registered;  //USER+NICK+PASS

public:
	Client();
	Client(int fd);
	Client(int fd, std::string nickname, std::string receivedData);
	Client(const Client &o);
	Client &operator=(const Client &o);
	~Client();

	//__________GETTERs______________________
	// to get the file descriptor of the client
	int getFd() const;
	// to get the nickname of the client
	const std::string &getNickname() const;
	//to get the username of the client
	std::string getUser() const;

	//to check if the client is registered
	bool isRegistered() const;
	//to check if the client is an operator
	bool isOperator() const;
	//to join a channel
	void joinChannel(const std::string& channel);
	//to leave a channel
	void leaveChannel(const std::string& channel);
	//to add an operator to the channel
	void addOperator(const std::string& channel);
	//to remove an operator from the channel
	void removeOperator(const std::string& channel);
	
	const std::string &getReceivedData() const;

	//_________SETTERs_________________
	//to set the nickname from the NICK command
	void setNickname(const std::string &nickname);
	// to set the user from the USER command
	void setUser(const std::string& user);
	
	
	

	void appendToReceivedData(const std::string &chunk);
	void clearReceivedData();
	
	//________________OTHER FUNCTIONS_________________________________
	bool hasCompleteCommandRN();
	std::string extractNextCmd();
};
const std::ostream &operator<<(std::ostream &os, const Client &o);