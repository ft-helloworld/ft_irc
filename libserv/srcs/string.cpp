/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 14:40:30 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 20:52:28 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <cstring>
#include <iomanip>

void String::SplitArguments(std::vector<const std::string>& args, const std::string& line)
{
	std::istringstream iss(line);
	std::string s;
	bool flag = false;
	std::string str;

	while (std::getline(iss, s, ' '))
	{
		if (!s.empty())
		{
			if (args.size() > 0)
				flag = true;
			if (flag && s[0] == ':')
			{
				str = s;
				break;
			}
			args.push_back(s);
		}
	}
	if (!str.empty())
	{
		str = line.substr(line.find(str) + 1);
		args.push_back(str);
	}
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

const std::string String::ItoCode(int number)
{
	std::ostringstream ss;
	ss << std::setw(3) << std::setfill('0') << number;
	return ss.str();
}

bool String::IsLetter(char ch) { return std::isalpha(ch); }

bool String::IsDigit(char ch) { return std::isdigit(ch); }

bool String::IsSpecial(char ch)
{
	const char* const specialChars = "[]\\`-^{|}";

	return std::memchr(specialChars, ch, sizeof(specialChars));
}
