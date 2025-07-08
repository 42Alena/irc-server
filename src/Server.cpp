/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:25 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/08 21:50:22 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

//======================== PRIVATE: CONSTRUCTORS  (1 server only) ===================//
Server::Server() {}
Server::Server(const Server &o) { (void)o; }
Server &Server::operator=(const Server &o)
{
	(void)o;
	return *this;
}

//======================== PUBLIC: CONSTRUCTORS & DESTRUCTORS ===================//

Server::Server(int port, const std::string &password) : _serverName("ircserv"),
														_port(port),
														_password(password),
														_serverFd(-1) {}

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
				removeClient(fd, pollFdIndex);
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

const std::string &Server::getServerName() const
{
	return _serverName;
}

//======================== PRIVATE: HELPER FUNCTIONS =========================//

// Command Dispatch & Parsing
void Server::handleCommand(Client *client, const std::string &line)
{
	std::cout << SRV << " Received command from client fd " << client->getFd() << ": " << line << RST << std::endl;

	// Create a stream from the input line to parse it
	std::istringstream iss(line);
	std::string command;
	iss >> command;

	// Ignore empty lines
	if (command.empty())
		return;
	// Convert command to uppercase (IRC commands are case-insensitive)
	std::transform(command.begin(), command.end(), command.begin(), ::toupper);

	// Parse the remaining tokens into arguments
	std::string arg;
	std::vector<std::string> arguments;

	while (iss >> arg)
		arguments.push_back(arg);

	// ---------- Handle IRC commands ----------

	// Basic communication
	if (command == "PING") // must reply with PONG
		handlePing(*this, *client, arguments);

	// Capability negotiation (minimal support for CAP LS)
	else if (command == "CAP")
	{
		if (arguments.size() > 0 && arguments[0] == "LS")
		{
			std::string message = "CAP * LS :\r\n";
			sendToClient(client->getFd(), message);
		}
	}
	// ---------- Authentication ----------
	else if (command == "PASS") // Set connection password
		handlePass(*this, *client, arguments);
	else if (command == "NICK") // Set nickname
		handleNick(*this, *client, arguments);
	else if (command == "USER") // Set username + real name
		handleUser(*this, *client, arguments);

	// ---------- Private + Channel Messaging ----------
	else if (command == "PRIVMSG") // Send private or channel message
		handlePrivateMessage(*this, *client, arguments);

	// ---------- Channel Interaction ----------
	else if (command == "JOIN") // join to the channel
		handleJoin(*this, *client, arguments);
	else if (command == "PART") // leave a channel
		handlePart(*this, *client, arguments);

	// ---------- Session / Disconnection ----------
	else if (command == "QUIT") // disconnect from the server: remove client+inform others
		handleQuit(*this, *client, arguments);

	// ---------- Channel Operator Commands ----------
	else if (command == "KICK") // kick a user from a channel
		handleKick(*this, *client, arguments);
	else if (command == "INVITE") // invite a user to a channel
		handleInvite(*this, *client, arguments);
	else if (command == "TOPIC") // change or view the topic
		handleTopic(*this, *client, arguments);
	else if (command == "MODE") // channel modes:i(invites),t(topic),k(password),o(privileges),l(limit)
		handleMode(*this, *client, arguments);
		
	// ---------- Unknown Command ----------
	else
		sendToClient(client->getFd(), "Unknown command: " + command + "\r\n"); // TODO
}

//======================== PRIVATE: Connection & Client Management ==================//

bool Server::isNicknameInUse(const std::string &nickname)
{
	std::map<int, Client *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return true;
	}
	return false;
}

Client *Server::getClientByNickname(const std::string &nickname)
{

	std::map<int, Client *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}

void Server::acceptNewClient()
{
	while (true) // accept() all pending  TCP connections
	{
		struct sockaddr_in clientAddress;
		socklen_t clientAddressrLen = sizeof(clientAddress);
		int clientFd = accept(_serverFd, reinterpret_cast<struct sockaddr *>(&clientAddress), &clientAddressrLen);

		// get client ip(need for welcome message)
		char ipStr[INET_ADDRSTRLEN];
		// Convert binary IP to C-style string
		inet_ntop(AF_INET, &(clientAddress.sin_addr), ipStr, INET_ADDRSTRLEN);
		std::string clientIP = ipStr;

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
			_clients[clientFd] = new Client(clientFd, clientIP);
			std::cout << "New client connected on fd " << clientFd << " (" << clientIP << ")" << std::endl;
		}

		// (pollfd) Add new client to poll() monitoring
		struct pollfd clientPollFd;
		clientPollFd.fd = clientFd;
		clientPollFd.events = POLLIN; // Monitor client for incoming data
		clientPollFd.revents = 0;
		_pollFds.push_back(clientPollFd);
	}
}

/*
 * Close socket
 * Remove client from map
 * Remove fd from poll list
 */
void Server::removeClient(int fd, size_t pollFdIndex)
{
	int fdCloseResult = close(fd);
	checkResult(fdCloseResult, "Failed to close client TCP connection on fd " + intToString(fd));

	logInfo("Closed client TCP connection on fd " + intToString(fd));

	_clients.erase(fd);
	_pollFds.erase(_pollFds.begin() + pollFdIndex);
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
		// TODO: change logic std::cout << "ERRNO=" << errno << "< bytes=" << readBytes << std::endl;
		if (errno == EAGAIN && errno == EWOULDBLOCK)
		{
			// Non-blocking: No data available, normal, skip
			pollFdIndex++;
			// continue;
		}
		else
			checkResult(readBytes, "Failed to read from client TCP connection");
		// pollFdIndex++;
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

//======================== PRIVATE: Channel management ==============================//

Channel *Server::getChannelByName(const std::string &channelName)
{
	std::map<std::string, Channel *>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
		return it->second;
	return NULL;
}

bool Server::isChannelName(const std::string &channelName)
{
	if (_channels.find(channelName) != _channels.end())
		return true;
	return false;
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
