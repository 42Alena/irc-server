/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:32:27 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/26 15:44:41 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Server.hpp"
#include "include/Client.hpp"
#include "include/Channel.hpp"
#include <cstdlib> // atoi

int parsePort(const char *arg)
{
	int port = std::atoi(arg);
	if (port <= 0 || port > 65535)
		throw std::runtime_error("Invalid port");
	return port;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "USAGE: ./ircserv <port> <password>" << std::endl;
		return EXIT_FAILURE;
	}

	Server server;
	try
	{
		int port = parsePort(argv[1]);
		std::string password = argv[2];
		server = Server(port, password);
		server.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << server << std::endl;
	return EXIT_SUCCESS;
};