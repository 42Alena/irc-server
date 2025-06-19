/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:16 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/19 11:46:59 by akurmyza         ###   ########.fr       */
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
	std::string _nickname;	   // NICK, private msg
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
	int getFd() const;

	const std::string &getNickname() const;
	const std::string &getReceivedData() const;

	//_________SETTERs_________________
	void setNickname(const std::string &nickname);

	void appendToReceivedData(const std::string &chunk);
	void clearReceivedData();
	
	//________________OTHER FUNCTIONS_________________________________
	bool hasCompleteCommandRN();
	std::string extractNextCmd();
};
const std::ostream &operator<<(std::ostream &os, const Client &o);