/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:22 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/18 12:29:37 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <string>

#include <cerrno>//Errors
#include <cstring>//Errors

#include <sys/socket.h> //socket,bind,listen
#include <sys/types.h>    //socket
#include <sys/socket.h>

#include <fcntl.h> //fcntl
#include <netinet/in.h> //struct sockaddr_in(IPv4)
#include <arpa/inet.h> //htons(convert hot<=>bytes)
#include <poll.h> //poll
#include <map> //std::map https://en.cppreference.com/w/cpp/container/map.html
#include <cstdlib>  //EXIT_SUCCESS, EXIT_FAILURE

#include "Client.hpp"

class Server{
	private:
	
	int _port;
	std::string _password;
	int _server_fd;    //socket()
	// std::vector<struct pollfd> _poll_fds; 
	std::map<int, Client> _clients;
	
	public:
	Server();
	Server(int &port, std::string &password);
	Server(const Server &o);
	Server &operator=(const Server &o);
	~Server();

	int run();
	
	//getter
	int getPort() const;
	std::string getPassword() const;
};
std::ostream& operator<<(std::ostream &os, const Server &o );