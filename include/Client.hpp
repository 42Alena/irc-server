/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:16 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/27 17:36:24 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

//======================== SYSTEM HEADERS ====================================//
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

//======================== PROJECT HEADERS ===================================//
#include "../include/Server.hpp"
#include "../include/Channel.hpp"

//======================== CLASS DEFINITION ==================================//
class Client
{
private:
    //======================== PRIVATE: ATTRIBUTES =============================//

    int _fd;                                    // Client socket file descriptor (-1 = no socket)
    std::string _nickName;                     // Nickname set by NICK command
    std::string _userName;                     // Username set by USER command
    bool _registered;                          // True if USER+NICK+PASS provided

    std::vector<std::string> _channels;        // Channels the client has joined
    std::map<std::string, bool> _channelOps;   // Channels where client is operator

    std::string _sendData;                     // Buffer for outgoing data (accumulates until \r\n)
    std::string _receivedData;                 // Buffer for incoming data (accumulates until \r\n)

public:
    //======================== PUBLIC: CONSTRUCTORS & DESTRUCTORS ==============//
    Client();
    Client(int fd);
    Client(int fd, std::string nickname, std::string receivedData);
    Client(const Client &o);
    Client &operator=(const Client &o);
    ~Client();

    //======================== PUBLIC: GETTERS =================================//
    int getFd() const;                         // Get client socket fd
    std::string getNickname() const;           // Get client nickname
    std::string getUserName() const;           // Get client username
    const std::string &getReceivedData() const;// Get accumulated received data

    //======================== PUBLIC: SETTERS =================================//
    void setNickname(const std::string &nickname); // Set nickname (NICK cmd)
    void setUser(const std::string &user);         // Set username (USER cmd)

    //======================== PUBLIC: DATA BUFFER FUNCTIONS ===================//
    void appendToReceivedData(const std::string &chunk); // Add data to received buffer
    void clearReceivedData();                            // Clear received buffer

    //======================== PUBLIC: COMMAND PARSING =========================//
    bool hasCompleteCommandRN();               // Check if a complete command (\r\n) exists
    std::string extractNextCmd();              // Extract next complete command

    //======================== PUBLIC: CHANNEL FUNCTIONS =======================//
    bool isRegistered() const;                 // Check if fully registered (USER+NICK+PASS)
    bool isOperator(const std::string &channel) const; // Check if operator in given channel
    void joinChannel(const std::string &channel);      // Join a channel
    void leaveChannel(const std::string &channel);     // Leave a channel
    void addOperator(const std::string &channel);      // Add operator status in a channel
    void removeOperator(const std::string &channel);   // Remove operator status from a channel
};

//======================== NON-MEMBER OPERATORS ===============================//
const std::ostream &operator<<(std::ostream &os, const Client &o);
