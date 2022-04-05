/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircstring.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seungyel <seungyel@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 18:25:46 by smun              #+#    #+#             */
/*   Updated: 2022/04/05 21:18:58 by seungyel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircstring.hpp"
#include <algorithm>
#include <string>
#include <cstddef>
#include <sstream>

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

bool IRCString::IsChannelPrefix(char ch)
{
    return ch == '#' || ch == '&';
}

bool IRCString::IsValidChstring(const std::string& nick)
{
    // <any 8bit code except SPACE, BELL, NUL, CR, LF and comma (',')>
    return std::find_if_not(nick.begin(), nick.end(), &is_valid_chstring_char) == nick.end();
    //채널명은 최대 200자의 문자열('&' 또는 '#'자로 시작)입니다. 첫 번째 문자가 '&' 또는 '#'이라는 조건과는 별도로 채널 이름에 대한 유일한 제한은 공백('), 컨트롤 G(^G 또는 ASCII 7), 쉼표('프로토콜에 의해 목록 항목 구분자로 사용됨)를 포함할 수 없다는 것입니다.
}

void    IRCString::SplitTargets(std::set<const std::string>& targets, const std::string& str)
{
    std::istringstream iss(str);

    std::string elem;
    while (std::getline(iss, elem, ','))
        targets.insert(elem);
}
