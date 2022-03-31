/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircmessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 13:45:47 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 21:02:16 by smun             ###   ########.fr       */
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

private:
    const std::string   _prefix;
    const IRCCommand    _cmd;
    ParamVector         _params;
    const std::string   _trailing;

    IRCMessage& operator= (const IRCMessage&);

    static const IRCMessage Empty;

public:
    IRCMessage();
    ~IRCMessage();
    IRCMessage(const IRCMessage& msg);

    IRCMessage(const IRCCommand& cmd);
    IRCMessage(const IRCCommand& cmd, const std::string& trailing);
    IRCMessage(const std::string& prefix, const IRCCommand& cmd, const std::string& trailing);

    void    AddParam(const std::string& param);

    ParamVectorConstIterator    BeginParam() const;
    ParamVectorConstIterator    EndParam() const;
    ParamVector::size_type      SizeParam() const;
    const std::string&          GetParam(ParamVector::size_type i) const;
    const std::string&  GetCommand() const;
    const std::string&  GetTrailing() const;
    const std::string&  GetPrefix() const;
    bool    IsEmpty() const;

    const std::string   GetMessage() const;

    static IRCMessage Parse(const std::string& line);
};

#endif
