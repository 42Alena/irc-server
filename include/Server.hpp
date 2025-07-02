/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:22 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/02 10:46:10 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

//======================== SYSTEM HEADERS ====================================//
#include <iostream>
#include <sstream> // std::stringstream for intToString
#include <string>
#include <vector>
#include <map>       // std::map
#include <algorithm> // std::transform
#include <cstdlib>   // EXIT_SUCCESS, EXIT_FAILURE
#include <cctype>   // isalpha, isalnum, isprint

#include <cerrno>   // errno
#include <cstring>  // strerror
#include <cstdio>   // printf, perror (if needed)
#include <unistd.h> // read(), close()

#include <sys/types.h>  // socket types
#include <sys/socket.h> // socket(), bind(), listen(), accept()
#include <netinet/in.h> // struct sockaddr_in, htons()
#include <arpa/inet.h>  // inet_ntoa etc.
#include <poll.h>       // poll(), struct pollfd
#include <fcntl.h>      // fcntl()

//======================== PROJECT HEADERS ===================================//
#include "../include/colors.hpp"
#include "../include/replies.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"

//======================== CLASS DEFINITION ==================================//
class Server
{
private:
    //======================== PRIVATE: ATTRIBUTES =============================//

    // Server Configuration
    const std::string _serverName;
    int _port;
    std::string _password;

    // Socket & Poll Management
    int _serverFd;
    std::vector<struct pollfd> _pollFds;

    // Client & Channel Tracking
    std::map<int, Client *> _clients;
    std::map<std::string, Channel *> _channels;

    //======================== PRIVATE: HELPER FUNCTIONS =======================//

    // ─────────────────── Connection & Client Management ───────────────────
    bool isNicknameInUse(const std::string &nickname);
    Client *getClientByNickname(const std::string &nickname);
    bool isValidNickname(const std::string &nickname);
    bool isValidPassword(const std::string &password);

    void acceptNewClient();
    void removeClient(int fd, size_t pollFdIndex);
    void handleClientInput(size_t pollFdIndex);
    void sendToClient(int fd, const std::string &message);

    // ─────────────────── Channel Management ───────────────────────
    Channel *getChannelByName(const std::string &channelName);
    bool isChannelName(const std::string &channelName);

    // ─────────────────── Command Dispatch & Parsing ───────────────────
    void handleCommand(Client *client, const std::string &line);

    // ─────────────────── Mandatory IRC Command Handlers
    void handlePass(Client *client, const std::vector<std::string> &params);
    void handleNick(Client *client, const std::vector<std::string> &params);
    void handleUser(Client *client, const std::vector<std::string> &params);
    void handleJoin(Client *client, const std::vector<std::string> &params);
    void handlePrivateMessage(Client *client, const std::vector<std::string> &params);

    // ─────────────────── Internal Utilities ───────────────────
    void checkResult(int result, const std::string &errMsg);
    void logInfo(const std::string &msg);
    void logErrAndThrow(const std::string &msg);

    /* Optional for full channel logic or bonus commands

    void broadcastToChannel(const std::string &channel, const std::string &msg, int except_fd = -1);
    void handleKick(Client *client, const std::vector<std::string> &params);
    void handleInvite(Client *client, const std::vector<std::string> &params);
    void handleTopic(Client *client, const std::vector<std::string> &params);
    void handleMode(Client *client, const std::vector<std::string> &params);
    */

public:
    //======================== PUBLIC: CONSTRUCTORS & DESTRUCTORS ==============//
    Server();
    Server(int &port, std::string &password);
    Server(const Server &o);
    Server &operator=(const Server &o);
    ~Server();
    
    //======================== PUBLIC: MAIN SERVER METHODS =====================//
    int run();
    
    //======================== PUBLIC: GETTERS =================================//
    int getPort() const;
    std::string getPassword() const;

    //======================== PUBLIC: HELPER FUNCTIONS ========================//
    std::string intToString(int n);
};

//======================== NON-MEMBER OPERATORS ==============================//
std::ostream &operator<<(std::ostream &os, const Server &o);
