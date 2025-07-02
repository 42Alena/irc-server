/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:32:27 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/02 21:35:54 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Server.hpp"
#include "include/Client.hpp"
#include "include/Channel.hpp"
#include <cstdlib> // atoi

std::pair<int, std::string> parsePortPassword(const char *portArg, const char *passwordArg)
{
    char *endptr;
    long port = std::strtol(portArg, &endptr, 10);

    if (*endptr != '\0')
        throw std::runtime_error("Invalid port: contains non-numeric characters");

    if (port <= 0 || port > 65535)
        throw std::runtime_error("Invalid port: out of range");

    std::string password = passwordArg;
    if (!Server::isValidPassword(password))
        throw std::runtime_error("Invalid password");

    return std::make_pair(static_cast<int>(port), password);
}


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "USAGE: ./ircserv <port> <password>" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        std::pair<int, std::string> result = parsePortPassword(argv[1], argv[2]);

        Server server(result.first, result.second);
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
