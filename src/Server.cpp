/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:25 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/30 17:45:37 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

//======================== PUBLIC: CONSTRUCTORS & DESTRUCTORS ===================//

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

//======================== PUBLIC: MAIN SERVER METHODS ==========================//
int Server::run()
{
	//==================== SOCKET CREATION AND SETUP ====================//
	// (socket) Create server socket with TCP (SOCK_STREAM) over IPv4 (AF_INET)
	_serverFd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
	checkResult(_serverFd, "Failed to create server socket with TCP");

	// (setsockopt) Allow immediate reuse of port (SO_REUSEADDR) to avoid 'Address already in use' on restart
	int enableReuseAddress = 1;
	int serverSockOptResult = setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuseAddress, sizeof(enableReuseAddress)); // Allow quick restart on same port
	checkResult(serverSockOptResult, "Failed to set server socket option SO_REUSEADDR");

	// (fcntl) Set server socket to non-blocking mode (O_NONBLOCK) so poll() doesn't block indefinitely
	int serverFileControlResult = fcntl(_serverFd, F_SETFL, O_NONBLOCK); // Set socket to non-blocking mode
	checkResult(serverFileControlResult, "Failed to set server socket to non-blocking mode");

	// (sockaddr_in) Define server address for IPv4 (AF_INET), bind to all interfaces (INADDR_ANY) and target port
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(_port);		// Set port
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces

	// (bind) Bind server socket to IP/Port for incoming TCP connections
	int serverBindResult = bind(_serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)); // Bind socket to address
	checkResult(serverBindResult, "Failed to bind server socket to port " + intToString(_port));

	// (listen) Start listening for incoming TCP connections with max queue (SOMAXCONN)
	int serverListenResult = listen(_serverFd, SOMAXCONN); // Start listening for connections
	checkResult(serverListenResult, "Failed to listen on server port " + intToString(_port));

	//==================== POLL SETUP ====================//

	std::cout << "Registering server socket with poll() to monitor for new TCP connections..." << std::endl;
	struct pollfd serverPollFd;
	serverPollFd.fd = _serverFd;
	serverPollFd.events = POLLIN; // Interested in incoming connections
	serverPollFd.revents = 0;
	_pollFds.push_back(serverPollFd);

	//==================== MAIN SERVER LOOP to hanndle events====================//

	while (true)
	{
		// (poll) Wait for events on all monitored file descriptors
		int serverPollResult = poll(_pollFds.data(), _pollFds.size(), 0); // Wait for events on all fds
		checkResult(serverPollResult, "Failed to poll() while waiting for events");

		nfds_t pollFdIndex = 0;
		while (pollFdIndex < _pollFds.size())
		{
			//==================== NEW CLIENT CONNECTION HANDLING ====================//
			if (_pollFds[pollFdIndex].fd == _serverFd && (_pollFds[pollFdIndex].revents & POLLIN))
			{
				acceptNewClient();
				pollFdIndex++;
			}

			//==================== (close) CLIENT DATA HANDLING ====================//
			else if (_pollFds[pollFdIndex].revents & POLLIN)
			{
				handleClientInput(pollFdIndex);
			}
			//==================== CLIENT DISCONNECT OR ERROR ====================//

			else if (_pollFds[pollFdIndex].revents & (POLLHUP | POLLERR))
			{
				int fd = _pollFds[pollFdIndex].fd;
				int fdCloseResult = close(fd); // Close socket
				checkResult(fdCloseResult, "Failed to close client TCP connection on fd " + intToString(fd));
				std::cout << "Closed client TCP connection on fd " << fd << std::endl;

				_clients.erase(fd);
				_pollFds.erase(_pollFds.begin() + pollFdIndex);
			}
			//==================== NO EVENTS, SKIP ====================//

			else
			{
				pollFdIndex++;
			}
		}
	}

	return EXIT_SUCCESS;
}

//======================== PUBLIC: GETTERS ====================================//

int Server::getPort() const
{
	return _port;
}

std::string Server::getPassword() const
{
	return _password;
}

//======================== NON-MEMBER OPERATORS ===============================//

std::ostream &operator<<(std::ostream &os, const Server &o)
{
	os << "Port: " << o.getPort() << std::endl;
	os << "Password: [hidden]" << std::endl;
	os << "Password: " << o.getPassword() << std::endl; // TODO: hide. ONLY FOR DEBUG now
	return os;
}

//======================== PRIVATE: HELPER FUNCTIONS =========================//

// Command Dispatch & Parsing
void Server::handleCommand(Client *client, const std::string &line)
{
	std::cout << SRV << " Received command from client fd " << client->getFd() << ": " << line << RST << std::endl;

	std::istringstream iss(line);
	std::string command;
	iss >> command;

	if (command.empty())
		return;

	std::transform(command.begin(), command.end(), command.begin(), ::toupper);

	std::string arg;
	std::vector<std::string> arguments;

	while (iss >> arg)
		arguments.push_back(arg);

	// Basic test dispatch
	if (command == "PASS")
		handlePass(client, arguments);
	else if (command == "NICK")
		handleNick(client, arguments);
	else if (command == "USER")
		handleUser(client, arguments);
	else if (command == "JOIN")
		handleJoin(client, arguments);
	else
		sendToClient(client->getFd(), "Unknown command: " + command + "\r\n");
}

//======================== PRIVATE: Connection & Client Management ==================//

void Server::acceptNewClient()
{
	while (true) // accept() all pending  TCP connections
	{
		struct sockaddr_in clientAddress;
		socklen_t clientAddressrLen = sizeof(clientAddress);
		int clientFd = accept(_serverFd, reinterpret_cast<struct sockaddr *>(&clientAddress), &clientAddressrLen);

		if (clientFd == -1)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK) // Real error, not just no more connections
			{
				logErrAndThrow("Failed to accept new TCP connection");
			}
			break; // No more pending connections
		}

		// (fcntl) Set client socket to non-blocking mode
		int clientFileControlResult = fcntl(clientFd, F_SETFL, O_NONBLOCK); // Set client to non-blocking
		checkResult(clientFileControlResult, "Failed to set client socket to non-blocking mode");

		// (map) Add new client to _clients tracking
		if (_clients.find(clientFd) != _clients.end()) // Sanity check for logic error
		{
			logErrAndThrow("Logic error - client fd already exists in _clients map");
		}
		else
		{
			_clients[clientFd] = new Client(clientFd, "", ""); // Create new client object
			std::cout << "New client connected on fd " << clientFd << std::endl;
		}

		// (pollfd) Add new client to poll() monitoring
		struct pollfd clientPollFd;
		clientPollFd.fd = clientFd;
		clientPollFd.events = POLLIN; // Monitor client for incoming data
		clientPollFd.revents = 0;
		_pollFds.push_back(clientPollFd);
	}
}

void Server::removeClient(int fd, size_t pollFdIndex)
{
	close(fd);										// Close socket
	_clients.erase(fd);								// Remove client from map
	_pollFds.erase(_pollFds.begin() + pollFdIndex); // Remove fd from poll list
}

void Server::handleClientInput(size_t pollFdIndex)
{
	//(read) Incoming data from client over TCP connection
	int fd = _pollFds[pollFdIndex].fd;
	char buf[512];
	ssize_t readBytes = read(fd, buf, sizeof(buf)); // Read incoming data

	if (readBytes == 0)
	{
		logInfo("Client on fd " + intToString(fd) + " disconnected");
		removeClient(fd, pollFdIndex);
	}
	else if (readBytes > 0)
	{
		std::string receivedChunk(buf, readBytes);
		_clients[fd]->appendToReceivedData(receivedChunk); // Buffer data for command parsing

		while (_clients[fd]->hasCompleteCommandRN()) // Process all complete commands
		{
			std::string cmdClient = _clients[fd]->extractNextCmd();
			handleCommand(_clients[fd], cmdClient);
		}

		pollFdIndex++;
	}
	else
	{
		checkResult(readBytes, "Failed to read from client TCP connection");
		pollFdIndex++;
	}
}

/*
	   send - send a message on a socket
	   ssize_t send(int sockfd, const void buf[.size], size_t size, int flags);
	   https://man7.org/linux/man-pages/man2/send.2.html
		data()- return a const char * (pointer to the beginning of the message)
	   */
void Server::sendToClient(int fd, const std::string &message)
{
	int sendMsgResult = send(fd, message.data(), message.size(), 0);
	checkResult(sendMsgResult, "Failed to send a message on socket");
}

//======================== PRIVATE: Mandatory IRC Command Handlers ==================//

void Server::handlePass(Client *client, const std::vector<std::string> &params)
{
	(void)client;
	(void)params;

	sendToClient(client->getFd(), "PASS received\r\n");
}


/* 
https://www.rfc-editor.org/rfc/rfc1459.html
"4.1.2 Nick message

      Command: NICK
   Parameters: <nickname> [ <hopcount> ]

   NICK message is used to give user a nickname or change the previous
   one.  The <hopcount> parameter is only used by servers to indicate
   how far away a nick is from its home server.  A local connection has
   a hopcount of 0.  If supplied by a client, it must be ignored.

   If a NICK message arrives at a server which already knows about an
   identical nickname for another client, a nickname collision occurs.
   As a result of a nickname collision, all instances of the nickname
   are removed from the server's database, and a KILL command is issued
   to remove the nickname from all other server's database. If the NICK
   message causing the collision was a nickname change, then the
   original (old) nick must be removed as well.

   If the server recieves an identical NICK from a client which is
   directly connected, it may issue an ERR_NICKCOLLISION to the local
   client, drop the NICK command, and not generate any kills.



Oikarinen & Reed                                               [Page 14]


RFC 1459              Internet Relay Chat Protocol              May 1993


   Numeric Replies:

           ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
           ERR_NICKNAMEINUSE               ERR_NICKCOLLISION

   Example:

   NICK Wiz                        ; Introducing new nick "Wiz".

   :WiZ NICK Kilroy                ; WiZ changed his nickname to Kilroy.
   "*/
void Server::handleNick(Client *client, const std::vector<std::string> &params)
{
	
	(void)client;
	(void)params;
	sendToClient(client->getFd(), "NICK received\r\n");
	
	sendToClient(client->getFd(), "SERVER: NICK received\r\n");
}

void Server::handleUser(Client *client, const std::vector<std::string> &params)
{
	(void)client;
	(void)params;
	sendToClient(client->getFd(), "USER received\r\n");
}

void Server::handleJoin(Client *client, const std::vector<std::string> &params)
{
	(void)client;
	(void)params;
	sendToClient(client->getFd(), "JOIN received\r\n");
}

void Server::handlePrivateMessage(Client *client, const std::vector<std::string> &params)
{
	(void)client;
	(void)params;
	logInfo("handlePrivateMessage() called - Not implemented yet.");
}

//======================== PRIVATE:Internal Utilities ==================//

void Server::checkResult(int result, const std::string &errMsg)
{
	if (result == -1)
		logErrAndThrow(errMsg);
}

void Server::logErrAndThrow(const std::string &msg)
{
	// prints: ESRV=  🤖🔥
	std::cerr << ESRV << msg << RST << std::endl;
	throw std::runtime_error("SERVER(errno):  " + std::string(strerror(errno)));
}

void Server::logInfo(const std::string &msg)
{
	// prints: SRV=  🤖🔥
	std::cout << BLU << SRV << " " << msg << RST << std::endl;
}

//======================== PUBLIC: HELPER FUNCTIONS ===========================//

std::string Server::intToString(int n)
{
	std::ostringstream os;
	os << n;
	return os.str();
}
