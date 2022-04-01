/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircstring.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 17:26:19 by smun              #+#    #+#             */
/*   Updated: 2022/04/02 02:18:34 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSTRING_HPP
#define IRCSTRING_HPP

#include <iosfwd>

class IRCString
{
private:
    IRCString();
    IRCString(const IRCString&);
    IRCString& operator= (const IRCString&);

public:
    ~IRCString();

    static bool IsValidNick(const std::string& nick);
    static bool IsChannelPrefix(char ch);
    static bool IsValidChstring(const std::string& nick);
};

#endif
