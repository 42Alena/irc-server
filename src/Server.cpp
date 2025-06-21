/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:25 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/21 10:00:40 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server() : _port(0),
				   _password(""),
				   _serverFd(-1) {}

Server::Server(int &port, std::string &password) : _port(port),
												   _password(password),
												   _serverFd(-1) {}
Server::Server(const Server &o) : _port(o._port),
								  _password(o._password),
								  _serverFd(o._serverFd) {}
Server &Server::operator=(const Server &o)
{
	if (this != &o)
	{
		_port = o._port;
		_password = o._password;
		_serverFd = o._serverFd;
	}
	return *this;
}

Server::~Server() {}

/*
Communication between client and server has to be done via TCP/IP (v4 or v6).
socket - create an endpoint for communication	            https://man7.org/linux/man-pages/man2/socket.2.html
getsockopt, setsockopt - get and set options on sockets 	https://man7.org/linux/man-pages/man2/setsockopt.2.html
fcntl - manipulate file descriptor                         	https://man7.org/linux/man-pages/man2/fcntl.2.html
bind - bind a name to a socket								https://man7.org/linux/man-pages/man2/bind.2.html


struct sockaddr_in {										// https://man7.org/linux/man-pages/man3/sockaddr.3type.html
	sa_family_t     sin_family;     // AF_INET/
	in_port_t       sin_port;       // Port number
	struct in_addr  sin_addr;       //IPv4 address  ;}
https://man7.org/linux/man-pages/man3/inet_aton.3.html

INADDR_ANY (0.0.0.0)              means any address for socket binding;
IPv4														https://man7.org/linux/man-pages/man7/ip.7.html

struct in_addr {	uint32_t       s_addr;     }; // address in network byte order
htons()  converts the unsigned short integer hostshort  from host byte order to network byte order.  https://man7.org/linux/man-pages/man3/htons.3.html

listen - listen for connections on a socket https://man7.org/linux/man-pages/man2/listen.2.html
poll - wait for some event on a file descriptor https://man7.org/linux/man-pages/man2/poll.2.html
		 struct pollfd {
			   int   fd;         // file descriptor/
			   short events;     //requested events
			   short revents;    // returned events
		   };

	accept -  a connection on a socket https://man7.org/linux/man-pages/man2/accept.2.html


*/
int Server::run()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0); // socket(Ipv4,TCP, default) Ret: fd||-1 +errno
	if (_serverFd == -1)
		throw std::runtime_error(std::string("Failed Socket creation: ") + strerror(errno));

	int enableReuseAddress = 1;
	//(socket fd,socket layer(not tcp), reuse port, &option, size (option)). ret 0|| -1 +errno
	int setSocketOptionResult = setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR,
										   &enableReuseAddress, sizeof(enableReuseAddress));
	if (setSocketOptionResult == -1)	
		throw std::runtime_error("Failed: set Socket Option");

	int serverFileControlResult = fcntl(_serverFd, F_SETFL, O_NONBLOCK);
	if (serverFileControlResult == -1)	
		throw std::runtime_error(std::string("Failed set file control: ") + strerror(errno));
	
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;			// IPv4
	serverAddress.sin_port = htons(_port);		// convert port to network byte order
	serverAddress.sin_addr.s_addr = INADDR_ANY; // bind to all interfaces (0.0.0.0)

	int bindResult = bind(_serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (bindResult == -1)
		throw std::runtime_error(std::string("Failed to bind: ") + strerror(errno));
	

	int listenResult = listen(_serverFd, SOMAXCONN); //( ,(backlog=maximum number of pending connections= SOMAXCONN=128))
	if (listenResult == -1)	
		throw std::runtime_error(std::string("Failed to listen: ") + strerror(errno));
	

	// poll(struct pollfd *fds,number fds, timeout=secToWait=0=NonBlocking=returnImmediatly). ret>=0 || -1 errno
	//.data() comes from  <vector>. Ret:  a pointer to the first element (T*)||  NULL if empty.
	// number fds: _pollFds.size()
	int pollResult = poll(_pollFds.data(), _pollFds.size(), 0);
	if (pollResult == -1)	
		throw std::runtime_error(std::string("Failed to wait for event with  poll(): ") + strerror(errno));
	

	struct pollfd server_pollfd;
	server_pollfd.fd = _serverFd;
	server_pollfd.events = POLLIN; // POLLIN - There is data to read
	server_pollfd.revents = 0;	   // 0 =poll(0fill with result)

	_pollFds.push_back(server_pollfd);

	//  Check which fds have events
	nfds_t fdIndex = 0;
	while (fdIndex < _pollFds.size())
	{

		if (_pollFds[fdIndex].revents & POLLIN)
		{
			char buf[10];
			ssize_t readBytes = read(_pollFds[fdIndex].fd, buf, sizeof(buf));
			if (readBytes == -1)
				throwRuntimeErr("read"); // Exit if read fails
			fdIndex++;
			printf("   Read %zd bytes: %.*s\n", readBytes, (int)readBytes, buf); // Show what was read

			// client
			if (_pollFds[fdIndex].fd == _serverFd)
			{
				int clientFd;
				struct sockaddr_in clientAddress;
				socklen_t clientAddressrLen = sizeof(clientAddress);
				clientFd = accept(_serverFd,
								  reinterpret_cast<struct sockaddr *>(&clientAddress), // Convert sockaddr_in* to sockaddr*
								  &clientAddressrLen);

				if (clientFd == -1)				
					throwRuntimeErr("creating client fd");
				
				int clientFileControlResult = fcntl(_serverFd, F_SETFL, O_NONBLOCK);
				if (clientFileControlResult == -1)				
					throw std::runtime_error(std::string("Failed set client file control: ") + strerror(errno));
				
			}
		}
	else if (_pollFds[fdIndex].revents & (POLLHUP | POLLERR)) // error socket || client disconnected
	{

		printf("    Closing fd %d\n", _pollFds[fdIndex].fd);
		if (close(_pollFds[fdIndex].fd) == -1)
		{
			_pollFds.erase(_pollFds.begin() + fdIndex);
			throwRuntimeErr("close");
		}
	}
	else
	{
		fdIndex++;
	}
}

/*
Infinite loop with poll()
If _serverFd is ready → accept() a new connection
Add the new client to _clients and _pollFds
If a client socket is ready → read or disconnect */

return EXIT_SUCCESS;
}

//________________getter_________________________________

int Server::getPort() const
{
	return _port;
}

std::string Server::getPassword() const
{
	return _password;
}

std::ostream &operator<<(std::ostream &os, const Server &o)
{

	os << "Port: " << o.getPort() << std::endl;
	os << "Password: [hidden]" << std::endl;

	os << "Password: " << o.getPassword() << std::endl; // TODO: hide. ONLY FOR DEBUG now
	return os;
}

//______ HELPER FUNCTIONS____________________
void Server::throwRuntimeErr(const std::string &errMsg)
{
	throw std::runtime_error("Error in " + errMsg + " => " + std::string(strerror(errno)));
}