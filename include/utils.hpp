/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akurmyza <akurmyza@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 18:20:44 by akurmyza          #+#    #+#             */
/*   Updated: 2025/07/03 18:29:31 by akurmyza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cctype>    // isalpha, isalnum, isprint
#include <string>
#include <iostream>
#include <sstream> // std::stringstream for intToString

//======================== HELPER FUNCTIONS ========================//

bool isValidPassword(const std::string &password);  
   
std::string intToString(int n);
