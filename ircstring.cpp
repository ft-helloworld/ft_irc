/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircstring.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 18:25:46 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 18:33:33 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircstring.hpp"
#include <algorithm>
#include <string>
#include <cstddef>

IRCString::~IRCString() {}

bool IRCString::IsValidNick(const std::string& nick)
{
    // <letter> { <letter> | <number> | <special> }

    for (size_t i = 0; i < nick.length(); ++i)
    {
        if (i == 0)
        {
            if (!String::IsLetter(nick[i]))
                return false;
        }
        else
        {
            if (!String::IsDigit(nick[i]) && !String::IsLetter(nick[i]) && !String::IsSpecial(nick[i]))
                return false;
        }
    }
    return true;
}

static bool is_valid_chstring_char(char ch)
{
    switch (ch)
    {
        case ' ':
        case '\0':
        case '\r':
        case '\n':
        case ',':
        case '\a':
            return false;
        default:
            return true;
    }
}

bool IRCString::IsValidChstring(const std::string& nick)
{
    // <any 8bit code except SPACE, BELL, NUL, CR, LF and comma (',')>
    return std::find_if_not(nick.begin(), nick.end(), &is_valid_chstring_char) == nick.end();
}
