/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:25 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/17 08:10:25 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

//======================== PRIVATE: CONSTRUCTORS  (1 server only) ===================//

/*
	Default constructor is intentionally disabled.
	A Server must be explicitly constructed with port and password.
*/
Server::Server() {}

/*
	Copy constructor.
	Copying and assignment are disabled for this class.
	IRC Server holds global state, sockets, and file descriptors — it must remain unique.
*/
Server::Server(const Server &o)
{
	logServerError("Copying a Server is forbidden: it owns global state, sockets, and runtime structures.");
	(void)o;
}

/*
	Assignment operator.
	Copying and assignment are disabled for this class.
	IRC Server holds global state, sockets, and file descriptors — it must not be duplicated.
*/
Server &Server::operator=(const Server &o)
{
	logServerError("Assigning a Server is forbidden: it owns sockets, poll state, and all live connections.");
	(void)o;
	return *this;
}

//======================== PUBLIC: CONSTRUCTORS & DESTRUCTORS ===================//

Server::Server(int port, const std::string &password) : _runningMainLoop(true),
														_serverName("ircserv"),
														_port(port),
														_password(password),
														_serverFd(-1) {}

Server::~Server()
{
	logServerInfo("Server destructor called.");

	// call shutdown if it wasnont already triggered manually by ctrl+c
	shutdown();
}

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

	logServerInfo("Registering server socket with poll() to monitor for new TCP connections...");
	struct pollfd serverPollFd;
	serverPollFd.fd = _serverFd;
	serverPollFd.events = POLLIN; // Interested in incoming connections
	serverPollFd.revents = 0;
	_pollFds.push_back(serverPollFd);

	//==================== MAIN SERVER LOOP to hanndle events====================//

	while (_runningMainLoop) // accept() all pending  TCP connections, when no SIGINT(CTRL+C)
	{
		// only one (poll) Wait for events on all monitored file descriptors
		int serverPollResult = poll(_pollFds.data(), _pollFds.size(), 0); // Wait for events on all fds. 0=nonblocking mode
		checkResult(serverPollResult, "Failed to poll() while waiting for events");

		nfds_t pollFdIndex = 0;
		while (pollFdIndex < _pollFds.size())
		{
			int fd = _pollFds[pollFdIndex].fd;

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
				pollFdIndex++;
			}

			//==================== CLIENT DISCONNECT OR ERROR ====================//
			else if (_pollFds[pollFdIndex].revents & (POLLHUP | POLLERR))
			{
				removeClient(fd, pollFdIndex);
				// not incrementing pollfd while removing client
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

/* called for normal exit or after after signal SIGINT(CTRL+C) */
void Server::shutdown()
{

	if (!_runningMainLoop) // prevent double shutdown
		return;

	logSeparateLine("💥Shutdown requested");
	logServerInfo(" Cleaning all server resources...");

	_runningMainLoop = false;

	// notifying all clients
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		sendToClient(it->first, "NOTICE * : Server is shutting down now\r\n");
	}


	// giving time for clients to receive message
	usleep(200000); // 200 ms

	// delete and close all clients
	while (!_clients.empty())
	{
		int fd = _clients.begin()->first;

		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			if (_pollFds[i].fd == fd)
			{
				removeClient(fd, i); //  handles close(), delete, erase from pollFds
				break;
			}
		}
	}

	// delete all channels
	for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		logServerInfo("Deleting channel: " + it->first);
		delete it->second;
	}
	_channels.clear();

	// clear pollfd
	_pollFds.clear();

	// close server socket
	if (_serverFd >= 0)
	{
		logServerInfo("Closing server socket fd " + intToString(_serverFd));
		close(_serverFd);
		_serverFd = -1;
	}

	logSeparateLine("Server shutdown complete");
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

	logServerInfo("Received command from client fd " + intToString(client->getFd()) + ": " + line);

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

//======================== PRIVATE: CONNECTION & CLIENT MANAGEMENT =============//
void Server::acceptNewClient()
{
	while (_runningMainLoop) // accept() all pending  TCP connections, when no SIGINT(CTRL+C)
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
			logServerInfo("New client connected on fd " + intToString(clientFd) + " (" + clientIP + ")");
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
   get the client object using their socket fd
   remove   client from all joined channels and delete  client object
	erase from client map and poll list
	close the socket
 */
void Server::removeClient(int fd, size_t pollFdIndex)
{
	// get the client object using their socket fd
	Client *client = _clients[fd];

	// remove   client from all joined channels and delete  client object
	removeClientFromAllChannels(client);

	delete client;

	// erase from client map and poll list
	_clients.erase(fd);
	_pollFds.erase(_pollFds.begin() + pollFdIndex);

	// close the socket
	int fdCloseResult = close(fd);
	checkResult(fdCloseResult, "Failed to close client TCP connection on fd " + intToString(fd));

	logServerInfo("Closed client TCP connection on fd " + intToString(fd));
}

void Server::handleClientInput(size_t pollFdIndex)
{
	//(read) Incoming data from client over TCP connection
	int fd = _pollFds[pollFdIndex].fd;
	if (_clients.count(fd) == 0)
		return; // fd not found — client was already removed or invalid

	char buf[512];
	ssize_t readBytes = read(fd, buf, sizeof(buf)); // Read incoming data

	if (readBytes == 0)
	{
		logServerInfo("Client on fd " + intToString(fd) + " disconnected");
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
	}
	else
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			// Non-blocking read: no data available — expected situation, skip
			return;
		}
		else
			checkResult(readBytes, "Failed to read from client TCP connection");
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

//======================== PUBLIC: CHANNEL UTILITIES ============================//
/* 
https://www.rfc-editor.org/rfc/rfc2812.txt
Channels names are strings (beginning with a '&', '#', '+' or '!'
   character) of length up to fifty (50) characters.  Apart from the
   requirement that the first character is either '&', '#', '+' or '!',
   the only restriction on a channel name is that it SHALL NOT contain
   any spaces (' '), 
   a control G (^G or ASCII 7), a comma (',').  Space
   is used as parameter separator and command is used as a list item
   separator by the protocol).  A colon (':') can also be used as a
   delimiter for the channel mask.  Channel names are case insensitive.
    '\a'= Control-G (^G). //caused a terminal to make a sound (a "ding").
*/
bool Server::isChannelName(const std::string &name)
{
	if (name.empty())                     // Reject empty string
		return false;

	char prefix = name[0];               // Get the first character
	if (prefix != '#' &&                 // Channel must start with
		prefix != '&' &&                // one of these: #, &, +, !
		prefix != '+' &&
		prefix != '!')
		return false;

	if (name.length() > 50)              // Reject names longer than 50 characters
		return false;

	for (size_t i = 1; i < name.length(); ++i)
	{
		char c = name[i];
		if (c == ' ' || c == ',' || c == '\a') // '\a' == ASCII 7 == ^G
			return false;
	}
	return true;                         // Valid channel name
}

bool Server::channelExists(const std::string &name)
{
	return _channels.find(name) != _channels.end();
}

Channel *Server::getChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return it->second;
	return NULL;
}

void Server::addChannel(const std::string &channelName, Client &creator)
{
	if (channelExists(channelName))
	{
		logErrAndThrow("Channel " + channelName + " already exists");
	}

	// Create new channel and add it to the server's channel map
	Channel *newChannel = new Channel(channelName, creator);
	_channels[channelName] = newChannel;

	logServerInfo("Created channel " + channelName + " by user " + creator.getNickname());
}

void Server::removeChannel(const std::string &channelName)
{

	std::map<std::string, Channel *>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
	{
		delete it->second;	 // deallocate the Channel object with its destructor
		_channels.erase(it); // remove pointer from the map
		logServerInfo("Channel " + channelName + " removed from server.");
	}
}

void Server::removeClientFromAllChannels(Client *client)
{
	if (!client)
		return;

	// copy to avoid using internal vector while modifying it
	const std::vector<Channel *> channelsCopy = client->getChannels();

	int fd = client->getFd();

	for (size_t i = 0; i < channelsCopy.size(); ++i)
	{
		Channel *ch = channelsCopy[i];

		if (!ch)
			continue; // extra safety

		logServerInfo("Removing client from channel: " + ch->getName());

		// remove client from channel
		ch->removeUser(fd, client);

		// if channel is empty after removal, delete it from server
		if (ch->getMembers().empty())
			removeChannel(ch->getName());
	}
}

//======================== PUBLIC: Internal Utilities ==================//

/*
In POSIX, most system calls (socket(), bind(), poll(), listen(), accept(), read(), etc.)
return:
	-1 on failure
	>= 0 on success
*/
void Server::checkResult(int result, const std::string &errMsg)
{
	if (result == -1)
		logErrAndThrow(errMsg);
}

void Server::logErrAndThrow(const std::string &msg)
{

	logServerError(msg);
	throw std::runtime_error("SERVER(errno):  " + std::string(strerror(errno)));
}
