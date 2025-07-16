/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 18:20:44 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/16 10:03:26 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cctype>    // isalpha, isalnum, isprint
#include <string>
#include <iostream>
#include <sstream> // std::stringstream for intToString
#include "colors.hpp"

//======================== PUBLIC: welcome to cIRCus             ==================//
void printWelcomeMessage();

//============================ LOGGER: GLOBAL LOGGING FOR ALL CLASSES ============================//
	
    void logServerInfo(const std::string &msg);
    void logServerError(const std::string &msg);

        
    void logClientInfo(const std::string &msg);   //cyan
    void logClientError(const std::string &msg); // red - error

    void logChannelInfo(const std::string &msg); // magenta - info
	void logChannelError(const std::string &msg); // red - error

    void logSeparateLine(const std::string &msg);

//======================== PUBLIC: NICKNAME & USERNAME VALIDATION ==================//
bool isValidNickname(const std::string &nickname);
bool isValidUsername(const std::string &username);

//======================== PUBLIC: PASSWORD VALIDATION =============================//
bool isValidPassword(const std::string &password);

//======================== PUBLIC: CONVERSION UTILITIES =============================//
std::string intToString(int n);