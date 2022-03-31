/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircmessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 13:45:47 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 18:40:30 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>
#include "irccommand.hpp"

class IRCMessage
{
public:
    typedef std::vector<std::string> ParamVector;
    typedef std::vector<std::string>::const_iterator ParamVectorConstIterator;

    const std::string   _prefix;
    const IRCCommand    _cmd;
    ParamVector         _params;
    const std::string   _trailing;

private:
    IRCMessage& operator= (const IRCMessage&);
    IRCMessage();

public:
    ~IRCMessage();
    IRCMessage(const IRCMessage& msg);

    IRCMessage(const IRCCommand& cmd);
    IRCMessage(const IRCCommand& cmd, const std::string& trailing);
    IRCMessage(const std::string& prefix, const IRCCommand& cmd, const std::string& trailing);

    void    AddParam(const std::string& param);
    const std::string   GetMessage() const;
};

#endif
