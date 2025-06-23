/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 17:27:02 by akurmyza          #+#    #+#             */
/*   Updated: 2025/06/23 18:16:20 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>

// _____Terminal Colors______
#define RED   "\033[1;31m"
#define GRN   "\033[1;32m"
#define YEL   "\033[1;33m"
#define BLU   "\033[1;34m"
#define MAG   "\033[1;35m"
#define CYN   "\033[1;36m"
#define WHT   "\033[1;37m"
#define ORG   "\033[1;38;5;208m"  
#define PNK   "\033[1;38;5;213m"  
#define AQU   "\033[1;38;5;14m"   
#define RST   "\033[0m"           

// __________Symbols_______________
const std::string  SRV  = "🤖 " ; // Server messages
const std::string  CLT = "👩‍💻 " ; // Client messages
const std::string  CHN  = "💬 " ; // Channel messages
const std::string  ERR = "🔥 ";  // Error messages
const std::string  OK  = "🎉 ";  // Success / Confirm

//________combined Symbols___________
const std::string ESRV = std::string(RED) + "🤖🔥"; 
const std::string ECLT = std::string(RED) + "👩‍💻🔥"; 
const std::string ECHN = std::string(RED) + "💬🔥"; 
 
