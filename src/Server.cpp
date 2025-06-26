/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:25 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/26 13:04:38 by akurmyza         ###   ########.fr       */
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

socket()	"Failed to create server socket"
bind()	"Failed to bind server socket to address"
listen()	"Failed to listen for incoming connections"
accept()	"Failed to accept new client connection"
fcntl()	"Failed to set socket to non-blocking mode"
*/
int Server::run()
{
	// (socket) Create server socket with TCP (SOCK_STREAM) over IPv4 (AF_INET)
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	checkResult(_serverFd, "SERVER: Failed to create server socket with TCP");

	// (setsockopt) Allow immediate reuse of port (SO_REUSEADDR) to avoid 'Address already in use' on restart
	int enableReuseAddress = 1;
	int serverSockOptResult = setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuseAddress, sizeof(enableReuseAddress));
	checkResult(serverSockOptResult, "SERVER: Failed to set server socket option SO_REUSEADDR");

	// (fcntl) Set server socket to non-blocking mode (O_NONBLOCK) so poll() doesn't block indefinitely
	int serverFileControlResult = fcntl(_serverFd, F_SETFL, O_NONBLOCK);
	checkResult(serverFileControlResult, "SERVER: Failed to set server socket to non-blocking mode");

	// (sockaddr_in) Define server address for IPv4 (AF_INET), bind to all interfaces (INADDR_ANY) and target port
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(_port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	// (bind) Bind server socket to IP/Port for incoming TCP connections
	int serverBindResult = bind(_serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	checkResult(serverBindResult, "SERVER: Failed to bind server socket to port " + std::to_string(_port));

	// (listen) Start listening for incoming TCP connections with max queue (SOMAXCONN)
	int serverListenResult = listen(_serverFd, SOMAXCONN);
	checkResult(serverListenResult, "SERVER: Failed to listen on server port " + std::to_string(_port));

	// (pollfd) Register server socket with poll() to monitor for new TCP connections (POLLIN)
	std::cout << "SERVER: Registering server socket with poll() to monitor for new TCP connections..." << std::endl;
	struct pollfd serverPollFd;
	serverPollFd.fd = _serverFd;
	serverPollFd.events = POLLIN;
	serverPollFd.revents = 0;
	_pollFds.push_back(serverPollFd);

	// (loop) Start main server loop to handle events
	while (true)
	{
		// (poll) Wait for events on all monitored file descriptors
		int serverPollResult = poll(_pollFds.data(), _pollFds.size(), 0);
		checkResult(serverPollResult, "SERVER: Failed to poll() while waiting for events");

		nfds_t fdIndex = 0;
		while (fdIndex < _pollFds.size())
		{
			// Check for readable events
			if (_pollFds[fdIndex].fd == _serverFd && (_pollFds[fdIndex].revents & POLLIN))
			{
				// (accept) Accept new incoming TCP connection
				struct sockaddr_in clientAddress;
				socklen_t clientAddressrLen = sizeof(clientAddress);
				int clientFd = accept(_serverFd, reinterpret_cast<struct sockaddr *>(&clientAddress), &clientAddressrLen);
				checkResult(clientFd, "SERVER: Failed to accept new TCP connection");

				// (fcntl) Set client socket to non-blocking mode
				int clientFileControlResult = fcntl(clientFd, F_SETFL, O_NONBLOCK);
				checkResult(clientFileControlResult, "SERVER: Failed to set client socket to non-blocking mode");

				// (pollfd) Add new client to poll() monitoring
				struct pollfd clientPollFd;
				clientPollFd.fd = clientFd;
				clientPollFd.events = POLLIN;
				clientPollFd.revents = 0;

				// (map) Add new client to _clients tracking
				if (_clients.find(clientFd) != _clients.end())
				{
					logErrAndThrow("SERVER: Logic error - client fd already exists in _clients map");
				}
				else
				{
					_clients[clientFd] = new Client(clientFd, "", "");
					std::cout << "SERVER: New client connected on fd " << clientFd << std::endl;
				}

				_pollFds.push_back(clientPollFd);
			}
			else if (_pollFds[fdIndex].revents & POLLIN)
			{
				// (read) Incoming data from client over TCP connection
				char buf[10];
				ssize_t readBytes = read(_pollFds[fdIndex].fd, buf, sizeof(buf));
				checkResult(readBytes, "SERVER: Failed to read from client TCP connection");
				std::cout << "SERVER: Received " << readBytes << " bytes from client fd " << _pollFds[fdIndex].fd << std::endl;
				fdIndex++;
			}
			else if (_pollFds[fdIndex].revents & (POLLHUP | POLLERR))
			{
				// (close) Handle client disconnection or socket error
				int fd = _pollFds[fdIndex].fd;
				int fdCloseResult = close(fd);
				checkResult(fdCloseResult, "SERVER: Failed to close client TCP connection on fd " + std::to_string(fd));
				std::cout << "SERVER: Closed client TCP connection on fd " << fd << std::endl;

				_clients.erase(fd);
				_pollFds.erase(_pollFds.begin() + fdIndex);
			}
			else
			{
				fdIndex++;
			}
		}
	}

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

void Server::checkResult(int result, const std::string &errMsg)
{
	if (result == -1)
		logErrAndThrow(errMsg);
}

void Server::logErrAndThrow(const std::string &msg)
{
	std::cerr << ESRV << msg << RST << std::endl;
	throw std::runtime_error("SERVER: failed to " + msg + std::string(strerror(errno)));
}

void Server::logInfo(const std::string &msg)
{
	std::cout << BLU << SRV << "SERVER:  " << msg << RST << std::endl;
}