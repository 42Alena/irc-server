/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:22 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/09 11:08:31 by akurmyza         ###   ########.fr       */
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
#include <cctype>    // isalpha, isalnum, isprint

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
#include "../include/commands.hpp"
#include "../include/utils.hpp"
#include "../include/replies.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/colors.hpp"

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

    //======================== PRIVATE: CONSTRUCTORS (Singleton-style prevention) ==//
    Server();
    Server(const Server &o);
    Server &operator=(const Server &o);

    //======================== PRIVATE: CONNECTION & CLIENT MANAGEMENT =============//
   
    void acceptNewClient();
    void removeClient(int fd, size_t pollFdIndex);
    void handleClientInput(size_t pollFdIndex);
    
    //======================== PRIVATE: CHANNEL MANAGEMENT =========================//
    Channel *getChannelByName(const std::string &channelName);
    
    //======================== PRIVATE: COMMAND DISPATCH & PARSING =================//
    void handleCommand(Client *client, const std::string &line);
    
    //======================== PRIVATE: INTERNAL UTILITIES =========================//
    void checkResult(int result, const std::string &errMsg);
    void logInfo(const std::string &msg);
    void logErrAndThrow(const std::string &msg);
    
    
    public:
    //======================== PUBLIC: CONSTRUCTORS & DESTRUCTOR ===================//
    Server(int port, const std::string &password);
    ~Server();
    
    //======================== PUBLIC: MAIN SERVER METHODS =========================//
    int run();
    
    //======================== PUBLIC: CLIENT UTILITIES ============================//
    Client *getClientByNickname(const std::string &nickname);
    void sendToClient(int fd, const std::string &message);

    bool isNicknameInUse(const std::string &nickname);
    
    //======================== PUBLIC: CHANNEL UTILITIES ============================//
    bool isChannelName(const std::string &channelName);
    bool channelExists(const std::string &name);
    Channel* getChannel(const std::string &name);

    //Luis ToDo: Add a method to add a new channel to the server when needed 
    // also a method to remove a channel when needed
    
    //======================== PUBLIC: GETTERS =====================================//
    int getPort() const;
    const std::string &getServerName() const;

};
