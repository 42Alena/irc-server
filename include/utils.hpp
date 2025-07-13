/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 18:20:44 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/13 13:33:19 by akurmyza         ###   ########.fr       */
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

//======================== PUBLIC: NICKNAME & USERNAME VALIDATION ==================//
bool isValidNickname(const std::string &nickname);
bool isValidUsername(const std::string &username);

//======================== PUBLIC: PASSWORD VALIDATION =============================//
bool isValidPassword(const std::string &password);

//======================== PUBLIC: CONVERSION UTILITIES =============================//
std::string intToString(int n);