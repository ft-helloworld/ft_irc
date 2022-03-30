/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yejsong <yejsong@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 14:40:30 by smun              #+#    #+#             */
/*   Updated: 2022/03/30 20:27:39 by yejsong          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cctype>

void String::SplitArguments(std::vector<const std::string>& args, const std::string& line)
{
    std::istringstream iss(line);
    std::string s;
    while (std::getline(iss, s, ' '))
        if (!s.empty())
            args.push_back(s);
}

int String::Stoi(const std::string& str)
{
    std::istringstream ss(str);
    int n;
    ss >> n;
    if (ss.fail())
        throw std::runtime_error("Not integer");
    return n;
}

std::string String::Join(
    std::vector<const std::string>::const_iterator begin,
    std::vector<const std::string>::const_iterator end)
{
    std::ostringstream oss;
    while (begin != end)
    {
        oss << *begin;
        ++begin;
        if (begin != end)
            oss << ' ';
    }
    return oss.str();
}

bool String::IsLetter(char ch) { return std::isalpha(ch); }
bool String::IsDigit(char ch) { return std::isdigit(ch); }

bool String::IsSpecial(char ch)
{
    const char* const specialChars = "[]\\`_^{|}";

    return std::memchr(specialChars, ch, sizeof(specialChars));
}