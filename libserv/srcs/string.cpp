/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yejsong <yejsong@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 14:40:30 by smun              #+#    #+#             */
/*   Updated: 2022/04/02 13:31:28 by yejsong          ###   ########.fr       */
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
			if (!flag)
				flag = true;
			else if (flag && s[0] == ':')
				str = s;
			if (str.empty())
				args.push_back(s);
		}
	}
	str = line.substr(line.find(str) + 1);
	args.push_back(str);
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
<<<<<<< HEAD

static bool CompareChar(char ch1, char ch2)
{
	if (ch1 == ch2)
		return true;
	if (std::isalpha(ch1) && std::isalpha(ch2))
		return std::tolower(ch1) == std::tolower(ch2);

	// the characters {}| are considered to be the lower case equivalents of the characters []\, respectively.
	if ((ch1 == '[' || ch1 == '{') && (ch2 == '[' || ch2 == '{'))
		return true;
	if ((ch1 == ']' || ch1 == '}') && (ch2 == ']' || ch2 == '}'))
		return true;
	if ((ch1 == '|' || ch1 == '\\') && (ch2 == '|' || ch2 == '\\'))
		return true;
	return false;
}

bool String::EqualIgnoreCase(const std::string& s1, const std::string& s2)
{
	return std::equal(s1.begin(), s1.end(), s2.begin(), CompareChar);
}
=======
>>>>>>> d3cbfa83d898d099687889157c3aab7f27612579
