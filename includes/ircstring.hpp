/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircstring.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 17:26:19 by smun              #+#    #+#             */
/*   Updated: 2022/04/02 16:15:56 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSTRING_HPP
#define IRCSTRING_HPP

#include <iosfwd>
#include <set>

class IRCString
{
private:
    IRCString();
    IRCString(const IRCString&);
    IRCString& operator= (const IRCString&);

public:
    typedef std::set<const std::string> TargetSet;

    ~IRCString();

    static bool IsValidNick(const std::string& nick);
    static bool IsChannelPrefix(char ch);
    static bool IsValidChstring(const std::string& nick);
    static void SplitTargets(std::set<const std::string>& targets, const std::string& str);
};

#endif
