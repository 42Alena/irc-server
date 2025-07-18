/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:16 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/18 16:15:18 by akurmyza         ###   ########.fr       */
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
#include "../include/utils.hpp"

//======================== CLASS DEFINITION ==================================//
class Client
{
private:
    //======================== PRIVATE: CONSTRUCTORS  (IRC clients are non-copyable)  ==============//
    Client(const Client &o);
    Client &operator=(const Client &o);
    
    //======================== PRIVATE: SOCKET & IDENTIFICATION =========================//
    int _fd;                // Client socket file descriptor (-1 = no socket)
    std::string _host;      // Client's IP address (used in welcome message)
    std::string _nickname;  // Nickname set by NICK command
    std::string _username;  // Username from USER command
    std::string _realname;  // Realname from USER command
    std::string _userModes; // Modes from USER command
    
    // luis: added the attribute here since will be needed it to join channels
    std::string _password; // Password set by PASS command (if any to use in channels when want to join)
    
    //======================== PRIVATE: REGISTRATION TRACKING ===========================//
    bool _hasProvidedPass; // True if PASS command provided
    bool _hasProvidedNick; // True if NICK command provided
    bool _hasProvidedUser; // True if USER command provided
    
    //======================== PRIVATE: CHANNEL MEMBERSHIP ================================//
    std::vector<Channel *> _channels;      // Channels the client has joined
    std::map<Channel *, bool> _channelOps; // Channels where client is operator
    
    //======================== PRIVATE: DATA BUFFERS ====================================//
    std::string _sendData;     // Buffer for outgoing data (accumulates until \r\n)
    std::string _receivedData; // Buffer for incoming data (accumulates until \r\n)
    
    
    public:
    //======================== PUBLIC: CONSTRUCTORS & DESTRUCTORS ==============//
    Client();
    Client(int fd, const std::string &host);
    ~Client();

    //======================== PUBLIC: GETTERS =================================//
    int getFd() const;               // Get client socket fd
    std::string getNickname() const; // Get client nickname
    std::string getUsername() const; // Get client username
    std::string getRealname() const; // Get client username
    std::string getHost() const;     // Get client IP address(used in welcome message)
    std::string getUserModes() const;
    std::string getPassword() const; // Get password set by PASS command
    std::string getPrefix() const;

    const std::string &getReceivedData() const;        // Get accumulated received data
    const std::vector<Channel *> &getChannels() const; // Get list of channels, where this client is a member

    bool getHasProvidedPass() const;
    bool getHasProvidedNick() const;
    bool getHasProvidedUser() const;

    //======================== PUBLIC: SETTERS =================================//
    void setNickname(const std::string &nickname); // Set nickname (NICK cmd)
    void setUsername(const std::string &username); // Set username (USER cmd)
    void setRealname(const std::string &realname); // Set user realname (USER cmd)
    void addUserMode(char modeChar);

    void setHasProvidedPass(bool val);
    void setHasProvidedNick(bool val);
    void setHasProvidedUser(bool val);

    //======================== PUBLIC: DATA BUFFER FUNCTIONS ===================//
    void appendToReceivedData(const std::string &chunk); // Add data to received buffer
    void clearReceivedData();                            // Clear received buffer

    //======================== PUBLIC: COMMAND PARSING =========================//
    bool hasCompleteCommandRN();  // Check if a complete command (\r\n) exists
    std::string extractNextCmd(); // Extract next complete command

    //======================== PUBLIC: CHANNEL FUNCTIONS =======================//
    bool isRegistered() const; // Check if fully registered (USER+NICK+PASS)
    bool isInChannel(const std::string &channelName) const;
    bool isOperator(Channel *channel) const; // Check if client is operator(flag '@') in given channel
    void joinChannel(Channel *channel);      // Join a channel
    void leaveChannel(Channel *channel);     // Leave a channel
    void addOperator(Channel *channel);      // Add operator status in a channel
    void removeOperator(Channel *channel);   // Remove operator status from a channel
};


