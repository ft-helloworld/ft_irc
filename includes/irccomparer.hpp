/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irccomparer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 19:56:32 by smun              #+#    #+#             */
/*   Updated: 2022/04/01 20:05:31 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCCOMPARER_HPP
#define IRCCOMPARER_HPP

#include <string>
#include <algorithm>

struct IRCComparer
{
    static bool CompareChar(char ch1, char ch2);

    inline bool operator()(const std::string& s1, const std::string& s2) const
    {
        return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(), CompareChar);
    }
};

#endif
