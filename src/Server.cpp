/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:25 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/27 18:28:17 by akurmyza         ###   ########.fr       */
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

	_serverFd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
	checkResult(_serverFd, "SERVER: Failed to create server socket with TCP");

	int enableReuseAddress = 1;
	int serverSockOptResult = setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuseAddress, sizeof(enableReuseAddress)); // Allow quick restart on same port
	checkResult(serverSockOptResult, "SERVER: Failed to set server socket option SO_REUSEADDR");

	int serverFileControlResult = fcntl(_serverFd, F_SETFL, O_NONBLOCK); // Set socket to non-blocking mode
	checkResult(serverFileControlResult, "SERVER: Failed to set server socket to non-blocking mode");

	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(_port);		// Set port
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces

	int serverBindResult = bind(_serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)); // Bind socket to address
	checkResult(serverBindResult, "SERVER: Failed to bind server socket to port " + intToString(_port));

	int serverListenResult = listen(_serverFd, SOMAXCONN); // Start listening for connections
	checkResult(serverListenResult, "SERVER: Failed to listen on server port " + intToString(_port));

	//==================== POLL SETUP ====================//

	std::cout << "SERVER: Registering server socket with poll() to monitor for new TCP connections..." << std::endl;
	struct pollfd serverPollFd;
	serverPollFd.fd = _serverFd;
	serverPollFd.events = POLLIN; // Interested in incoming connections
	serverPollFd.revents = 0;
	_pollFds.push_back(serverPollFd);

	//==================== MAIN SERVER LOOP ====================//

	while (true)
	{
		int serverPollResult = poll(_pollFds.data(), _pollFds.size(), 0); // Wait for events on all fds
		checkResult(serverPollResult, "SERVER: Failed to poll() while waiting for events");

		nfds_t fdIndex = 0;
		while (fdIndex < _pollFds.size())
		{
			//==================== NEW CONNECTION HANDLING ====================//

			if (_pollFds[fdIndex].fd == _serverFd && (_pollFds[fdIndex].revents & POLLIN))
			{
				while (true) // Accept all pending connections
				{
					struct sockaddr_in clientAddress;
					socklen_t clientAddressrLen = sizeof(clientAddress);
					int clientFd = accept(_serverFd, reinterpret_cast<struct sockaddr *>(&clientAddress), &clientAddressrLen);

					if (clientFd == -1)
					{
						if (errno != EAGAIN && errno != EWOULDBLOCK) // Real error, not just no more connections
						{
							logErrAndThrow("SERVER: Failed to accept new TCP connection");
						}
						break; // No more pending connections
					}

					int clientFileControlResult = fcntl(clientFd, F_SETFL, O_NONBLOCK); // Set client to non-blocking
					checkResult(clientFileControlResult, "SERVER: Failed to set client socket to non-blocking mode");

					if (_clients.find(clientFd) != _clients.end()) // Sanity check for logic error
					{
						logErrAndThrow("SERVER: Logic error - client fd already exists in _clients map");
					}
					else
					{
						_clients[clientFd] = new Client(clientFd, "", ""); // Create new client object
						std::cout << "SERVER: New client connected on fd " << clientFd << std::endl;
					}

					struct pollfd clientPollFd;
					clientPollFd.fd = clientFd;
					clientPollFd.events = POLLIN; // Monitor client for incoming data
					clientPollFd.revents = 0;
					_pollFds.push_back(clientPollFd);
				}

				fdIndex++;
			}
			//==================== CLIENT DATA HANDLING ====================//

			else if (_pollFds[fdIndex].revents & POLLIN)
			{
				int fd = _pollFds[fdIndex].fd;
				char buf[512];
				ssize_t readBytes = read(fd, buf, sizeof(buf)); // Read incoming data

				if (readBytes == 0)
				{
					std::cout << "SERVER: Client on fd " << fd << " disconnected." << std::endl;
					close(fd);									// Close socket
					_clients.erase(fd);							// Remove client from map
					_pollFds.erase(_pollFds.begin() + fdIndex); // Remove fd from poll list
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

					fdIndex++;
				}
				else
				{
					checkResult(readBytes, "SERVER: Failed to read from client TCP connection");
					fdIndex++;
				}
			}
			//==================== CLIENT DISCONNECT OR ERROR ====================//

			else if (_pollFds[fdIndex].revents & (POLLHUP | POLLERR))
			{
				int fd = _pollFds[fdIndex].fd;
				int fdCloseResult = close(fd); // Close socket
				checkResult(fdCloseResult, "SERVER: Failed to close client TCP connection on fd " + intToString(fd));
				std::cout << "SERVER: Closed client TCP connection on fd " << fd << std::endl;

				_clients.erase(fd);
				_pollFds.erase(_pollFds.begin() + fdIndex);
			}
			//==================== NO EVENTS, SKIP ====================//

			else
			{
				fdIndex++;
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
		sendToClient(client->getFd(), "SERVER: Unknown command: " + command + "\r\n");
}

//======================== PRIVATE: Connection & Client Management ==================//

void Server::acceptNewClient()
{
	logInfo("acceptNewClient() called - Not implemented yet.");
}

void Server::removeClient(int fd)
{
	(void)fd;
	logInfo("removeClient() called - Not implemented yet.");
}

void Server::handleClientInput(int fd)
{
	(void)fd;
	logInfo("handleClientInput() called - Not implemented yet.");
}

void Server::sendToClient(int fd, const std::string &message)
{
	(void)fd;
	(void)message;
	logInfo("sendToClient() called - Not implemented yet.");
}

//======================== PRIVATE: Mandatory IRC Command Handlers ==================//

void Server::handlePass(Client *client, const std::vector<std::string> &params)
{
	(void)client;
	(void)params;
	
	sendToClient(client->getFd(), "SERVER: PASS received\r\n");
}

void Server::handleNick(Client *client, const std::vector<std::string> &params)
{
	(void)client;
	(void)params;
	sendToClient(client->getFd(), "SERVER: NICK received\r\n");
}

void Server::handleUser(Client *client, const std::vector<std::string> &params)
{
	(void)client;
	(void)params;
	sendToClient(client->getFd(), "SERVER: USER received\r\n");
}

void Server::handleJoin(Client *client, const std::vector<std::string> &params)
{
	(void)client;
	(void)params;
	sendToClient(client->getFd(), "SERVER: JOIN received\r\n");
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
	std::cerr << ESRV << msg << RST << std::endl;
	throw std::runtime_error("SERVER(errno):  " + std::string(strerror(errno)));
}

void Server::logInfo(const std::string &msg)
{
	std::cout << BLU << SRV << "SERVER:  " << msg << RST << std::endl;
}

//======================== PUBLIC: HELPER FUNCTIONS ===========================//

std::string Server::intToString(int n)
{
	std::ostringstream os;
	os << n;
	return os.str();
}
