/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irccomparer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 20:04:48 by smun              #+#    #+#             */
/*   Updated: 2022/04/04 17:07:32 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>
#include "irccomparer.hpp"

bool IRCComparer::CompareChar(char ch1, char ch2)
{
    if (ch1 == ch2)
        return false;
    if (std::isalpha(ch1) && std::isalpha(ch2))
        return std::tolower(ch1) < std::tolower(ch2);

    // the characters {}| are considered to be the lower case equivalents of the characters []\, respectively.
    if ((ch1 == '[' || ch1 == '{') && (ch2 == '[' || ch2 == '{'))
        return false;
    if ((ch1 == ']' || ch1 == '}') && (ch2 == ']' || ch2 == '}'))
        return false;
    if ((ch1 == '|' || ch1 == '\\') && (ch2 == '|' || ch2 == '\\'))
        return false;
    return static_cast<unsigned char>(ch1) < static_cast<unsigned char>(ch2);
}
