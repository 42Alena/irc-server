/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:32:27 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/16 06:21:41 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Server.hpp"
#include "include/Client.hpp"
#include "include/Channel.hpp"
#include "include/utils.hpp"
#include <csignal> // For signal() https://en.cppreference.com/w/cpp/header/csignal.html
#include <cstdlib> // atoi,EXIT_FAILURE EXIT_SUCCESS

Server *globalRunningServer = NULL; // Global pointer to allow SIGINT access

void handleSigintCtrlC(int signum)
{
    (void)signum; //silence. need to be for no runtime issues
    if (globalRunningServer)
    {
        globalRunningServer->logError("💥 Received SIGINT (Ctrl-C). Shutting down...");
        globalRunningServer->shutdown();
    }
}

std::pair<int, std::string> parsePortPassword(const char *portArg, const char *passwordArg)
{
    printWelcomeMessage();
    char *endptr;
    long port = std::strtol(portArg, &endptr, 10);

    if (*endptr != '\0')
        throw std::runtime_error("Invalid port: contains non-numeric characters");

    if (port <= 0 || port > 65535)
        throw std::runtime_error("Invalid port: out of range");

    std::string password = passwordArg;
    if (!isValidPassword(password))
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

    // Signal handling setup
    signal(SIGINT, handleSigintCtrlC); // set handler for Ctrl-C (SIGINT) using my function handleSigintCtrlC()
    signal(SIGPIPE, SIG_IGN);          // ignore SIGPIPE to prevent crashes when writing to closed sockets

    try
    {
        std::pair<int, std::string> result = parsePortPassword(argv[1], argv[2]);

        Server server(result.first, result.second);
        globalRunningServer = &server; // 🔥 Required for SIGINT handler to work
        server.run();
        //cleanup normally, if not triggered by signal
        globalRunningServer->shutdown();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    globalRunningServer = NULL;

    return EXIT_SUCCESS;
}

