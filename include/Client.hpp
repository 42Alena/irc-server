/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:16 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/02 17:30:34 by akurmyza         ###   ########.fr       */
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

    //======================== SOCKET & IDENTIFICATION =========================//
    int _fd;                                    // Client socket file descriptor (-1 = no socket)
    std::string _nickName;                     // Nickname set by NICK command
    std::string _userName;                     // Username set by USER command

    //======================== REGISTRATION TRACKING ===========================//
    bool _hasProvidedPass;                     // True if PASS command provided
    bool _hasProvidedNick;                     // True if NICK command provided
    bool _hasProvidedUser;                     // True if USER command provided
    bool _registered;                          // True if full registration completed (PASS + NICK + USER)

    //======================== CHANNEL TRACKING ================================//
    std::vector<Channel*> _channels;           // Channels the client has joined
    std::map<Channel*, bool> _channelOps;      // Channels where client is operator

    //======================== DATA BUFFERS ====================================//
    std::string _sendData;                     // Buffer for outgoing data (accumulates until \r\n)
    std::string _receivedData;                 // Buffer for incoming data (accumulates until \r\n)

public:
    //======================== PUBLIC: CONSTRUCTORS & DESTRUCTORS ==============//
    Client();
    Client(int fd);
    Client(int fd, const std::string &nickname, const std::string &receivedData);
    Client(const Client &o);
    Client &operator=(const Client &o);
    ~Client();

    //======================== PUBLIC: GETTERS =================================//
    int getFd() const;                         // Get client socket fd
    std::string getNickname() const;           // Get client nickname
    std::string getUserName() const;           // Get client username
    const std::string &getReceivedData() const;// Get accumulated received data
    const std::vector<Channel*> &getChannels() const; // Get list of channels, where this client is a member

     bool getHasProvidedPass() const;
     bool getHasProvidedNick() const;
     bool getHasProvidedUser() const;
 
    
    

    //======================== PUBLIC: SETTERS =================================//
    void setNickname(const std::string &nickname); // Set nickname (NICK cmd)
    void setUser(const std::string &user);         // Set username (USER cmd)
    void setHasProvidedPass(bool val);
    void setHasProvidedNick(bool val);
    void setHasProvidedUser(bool val);

    
    //======================== PUBLIC: DATA BUFFER FUNCTIONS ===================//
    void appendToReceivedData(const std::string &chunk); // Add data to received buffer
    void clearReceivedData();                            // Clear received buffer

    //======================== PUBLIC: COMMAND PARSING =========================//
    bool hasCompleteCommandRN();               // Check if a complete command (\r\n) exists
    std::string extractNextCmd();              // Extract next complete command

    //======================== PUBLIC: CHANNEL FUNCTIONS =======================//
    bool isRegistered() const;                 // Check if fully registered (USER+NICK+PASS)
    bool isOperator(Channel* channel) const; // Check if operator in given channel
    void joinChannel(Channel* channel);      // Join a channel
    void leaveChannel(Channel* channel);     // Leave a channel
    void addOperator(Channel* channel);      // Add operator status in a channel
    void removeOperator(Channel* channel);   // Remove operator status from a channel
};

//======================== NON-MEMBER OPERATORS ===============================//
const std::ostream &operator<<(std::ostream &os, const Client &o);
