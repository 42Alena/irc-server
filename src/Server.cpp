/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:31:25 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/20 23:25:41 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server() : _port(0),
				   _password(""),
				   _server_fd(-1) {}

Server::Server(int &port, std::string &password) : _port(port),
												   _password(password),
												   _server_fd(-1) {}
Server::Server(const Server &o) : _port(o._port),
								  _password(o._password),
								  _server_fd(o._server_fd) {}
Server &Server::operator=(const Server &o)
{
	if (this != &o)
	{
		_port = o._port;
		_password = o._password;
		_server_fd = o._server_fd;
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
*/
int Server::run()
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0); // socket(Ipv4,TCP, default) Ret: fd||-1 +errno
	if (_server_fd == -1)
	{
		throw std::runtime_error(std::string("Failed Socket creation: ") + strerror(errno));
	}

	int enableReuseAddress = 1;
	//(socket fd,socket layer(not tcp), reuse port, &option, size (option)). ret 0|| -1 +errno
	int setSocketOptionResult = setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR,
										   &enableReuseAddress, sizeof(enableReuseAddress));
	if (setSocketOptionResult == -1)
	{
		throw std::runtime_error("Failed: set Socket Option");
	}

	int fileControlResult = fcntl(_server_fd, F_SETFL, O_NONBLOCK);
	if (fileControlResult == -1)
	{
		throw std::runtime_error(std::string("Failed set file control: ") + strerror(errno));
	}

	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;			// IPv4
	serverAddress.sin_port = htons(_port);		// convert port to network byte order
	serverAddress.sin_addr.s_addr = INADDR_ANY; // bind to all interfaces (0.0.0.0)

	int bindResult = bind(_server_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (bindResult == -1)
	{
		throw std::runtime_error(std::string("Failed to bind: ") + strerror(errno));
	}

	int listenResult = listen(_server_fd, SOMAXCONN); //( ,(backlog=maximum number of pending connections= SOMAXCONN=128))
	if (listenResult == -1)
	{
		throw std::runtime_error(std::string("Failed to listen: ") + strerror(errno));
	}

	// poll(struct pollfd *fds,number fds, timeout=secToWait=0=NonBlocking=returnImmediatly). ret>=0 || -1 errno
	//.data() comes from  <vector>. Ret:  a pointer to the first element (T*)||  NULL if empty.
	// number fds: _poll_fds.size()
	int pollResult = poll(_poll_fds.data(), _poll_fds.size(), 0);
	if (pollResult == -1)
	{
		throw std::runtime_error(std::string("Failed to wait for event with  poll(): ") + strerror(errno));
	}

	// for (int i =0; i <=  _poll_fds.size(); i++){
	// 	if(_poll_fds.revents() &&)
	// }

	// Add _server_fd to _poll_fds
	//  my : std::vector<struct pollfd> _poll_fds;
	// my: int _server_fd;    //sserver socket()

	struct pollfd server_pollfd;
	server_pollfd.fd = _server_fd;
	server_pollfd.events = POLLIN; // POLLIN - There is data to read
	server_pollfd.revents = 0;	   // 0 =poll(0fill with result)

	_poll_fds.push_back(server_pollfd);

	// Main loop + basic poll() usage	15–20 min	Includes poll(), loop structure, error check
	//  Check which fds have events

	nfds_t fdIndex = 0;
	while (fdIndex < _poll_fds.size())
	{

		if (_poll_fds[fdIndex].revents & POLLIN)
		{
			char buf[10];
			ssize_t readBytes = read(_poll_fds[fdIndex].fd, buf, sizeof(buf));
			if (readBytes == -1)
				throwRuntimeErr("read"); // Exit if read fails
			fdIndex++;
			printf("   Read %zd bytes: %.*s\n", readBytes, (int)readBytes, buf); // Show what was read
		}
		else if (_poll_fds[fdIndex].revents & (POLLHUP | POLLERR)) // error socket ||client disconnected
		{

			printf("    Closing fd %d\n", _poll_fds[fdIndex].fd);
			if (close(_poll_fds[fdIndex].fd) == -1)
			{
				_poll_fds.erase(_poll_fds.begin() + fdIndex);
				throwRuntimeErr("close"); // Exit if close fails
			}
		}
		else
		{
			fdIndex++;
		}
	}

	/*
	Infinite loop with poll()
	If _server_fd is ready → accept() a new connection
	Add the new client to _clients and _poll_fds
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