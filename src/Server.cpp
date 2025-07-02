/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:25 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/02 17:06:36 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

//======================== PUBLIC: CONSTRUCTORS & DESTRUCTORS ===================//

Server::Server() : _serverName("ircserv"),
				   _port(0),
				   _password(""),
				   _serverFd(-1) {}

Server::Server(int &port, std::string &password) : _serverName("ircserv"),
												   _port(port),
												   _password(password),
												   _serverFd(-1) {}

Server::Server(const Server &o) : _serverName(o._serverName),
								  _port(o._port),
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

	// WIP: Basic test dispatch
	if (command == "PASS")
		handlePass(client, arguments);
	else if (command == "NICK")
		handleNick(client, arguments);
	else if (command == "USER")
		handleUser(client, arguments);
	else if (command == "JOIN")
		handleJoin(client, arguments);
	else
		sendToClient(client->getFd(), "Unknown command: " + command + "\r\n"); // TODO
}

//======================== PRIVATE: Connection & Client Management ==================//

/*
https://www.rfc-editor.org/rfc/rfc1459.html#section-2.3.1
"
<nick>       ::= <letter> { <letter> | <number> | <special> }

<letter>     ::= 'a' ... 'z' | 'A' ... 'Z'
<number>     ::= '0' ... '9'
<special>    ::= '-' | '[' | ']' | '\' | '`' | '^' | '{' | '}'
..
1.2 Clients
nickname having a maximum length of nine (9) characters.
"
https://cplusplus.com/reference/cctype/
*/
bool Server::isValidNickname(const std::string &nickname)
{
	if (nickname.empty())
		return false;
	if (nickname.size() > 9)
		return false;

	if (!isalpha(nickname[0]))
		return false;

	for (std::string::size_type i = 1; i < nickname.size(); i++)
	{
		char c = nickname[i];
		if (isalpha(c) ||
			isdigit(c) ||
			c == '-' || c == '[' || c == ']' || c == '\\' || c == '`' || c == '^' || c == '{' || c == '}')
		{
			continue;
		}
		return false;
	}

	return true;
}

bool Server::isValidPassword(const std::string &password)
{
	if (password.empty())
		return false;
	if (password.size() > 32)
		return false;

	for (std::string::size_type i = 0; i < password.size(); i++)
	{
		char c = password[i];
		if (!isprint(c))
			return false;
	}

	return true;
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

//======================== PRIVATE: Mandatory IRC Command Handlers ==================//

/*
https://www.rfc-editor.org/rfc/rfc2812.html#section-3.1.2
Password message

	Command: PASS
   Parameters: <password>

   The PASS command is used to set a 'connection password'.  The
   optional password can and MUST be set before any attempt to register
   the connection is made.  Currently this requires that user send a
   PASS command before sending the NICK/USER combination.

   Numeric Replies:

	   ERR_NEEDMOREPARAMS                        ERR_ALREADYREGISTRED
		replyErr461NeedMoreParams()				replyErr462AlreadyRegistered()

		ERR_PASSWDMISMATCH
		replyErr464PasswordMismatch
   Example:

		   PASS secretpasswordhere
"
std::string replyErr461NeedMoreParams(const std::string &server, const std::string &command)
{
	return ":" + server + " " + ERR_NEEDMOREPARAMS + " " + command + " :Not enough parameters\r\n";
}
	std::string replyErr462AlreadyRegistered(const std::string &server)
{
	return ":" + server + " " + ERR_ALREADYREGISTRED + " :Unauthorized command (already registered)\r\n";
}
Typical raw IRC message from client:
PASS mysecretpassword
*/
void Server::handlePass(Client *client, const std::vector<std::string> &params)
{
	if (params.empty())
	{
		sendToClient(client->getFd(), replyErr461NeedMoreParams(_serverName, "PASS"));
		return;
	}

	if (!isValidPassword(params[0]))
	{
		sendToClient(client->getFd(), replyErr464PasswordMismatch(_serverName));
		return;
	}

	sendToClient(client->getFd(), "PASS received\r\n");
}

/*
	https://www.rfc-editor.org/rfc/rfc2812.html#section-3.1.2
"
Nick message
	Command: NICK
   Parameters: <nickname>
   NICK command is used to give user a nickname or change the existingone.

   Numeric Replies:

		   ERR_NONICKNAMEGIVEN
		   ERR_NICKNAMEINUSE
		   ERR_ERRONEUSNICKNAME

   Examples:
   NICK Wiz                ; Introducing new nick "Wiz" if session is
						   still unregistered, or user changing his
						   nickname to "Wiz"

   :WiZ!jto@tolsun.oulu.fi NICK Kilroy
						   ; Server telling that WiZ changed his
						   nickname to Kilroy.
"
 */
void Server::handleNick(Client *client, const std::vector<std::string> &params)
{
	if (params.empty())
	{
		sendToClient(client->getFd(), replyErr431NoNickGiven(_serverName));
		return;
	}

	const std::string newNickname = params[0];

	if (isNicknameInUse(newNickname))
	{
		sendToClient(client->getFd(), replyErr433NickInUse(_serverName, newNickname));
		return;
	}

	if (!isValidNickname(newNickname))
	{
		sendToClient(client->getFd(), replyErr432ErroneousNick(_serverName, newNickname));
		return;
	}

	const std::string oldNickname = client->getNickname();

	client->setNickname(newNickname);

	// broadcast after change nick to  client self and to all channles where this client is member
	//:<old_nick> NICK <new_nick>\r\n
	if (!oldNickname.empty())
	{
		// send message to client
		std::string messageNickChange = ":" + oldNickname + " NICK " + newNickname + "\r\n";
		sendToClient(client->getFd(), messageNickChange);

		// Send message to all clients in the same channels
		const std::vector<Channel *> &clientChannels = client->getChannels();

		for (std::vector<Channel *>::const_iterator it = clientChannels.begin(); it != clientChannels.end(); ++it)
		{
			(*it)->broadCastMessage(messageNickChange, client);
			std::cout << "Client is in channel: " << (*it)->getName() << std::endl;
		}
	}
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
