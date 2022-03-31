/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_exception.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 19:05:14 by yejsong           #+#    #+#             */
/*   Updated: 2022/04/01 01:35:04 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "irc_exception.hpp"
#include "ircmessage.hpp"
#include "irccommand.hpp"
#include <string>

irc_exception::~irc_exception() throw()
{
}

irc_exception::irc_exception(const irc_exception& o)
    : _message(o._message)
{
}

irc_exception::irc_exception(const IRCCommand& cmd) throw()
    : _message(HOSTNAME, cmd, "")
{
}

irc_exception::irc_exception(const IRCCommand& cmd, const std::string& trailing) throw()
    : _message(HOSTNAME, cmd, trailing)
{
}

irc_exception::irc_exception(const IRCCommand& cmd, const std::string& trailing, const std::string& param1) throw()
    : _message(HOSTNAME, cmd, trailing)
{
    _message.AddParam(param1);
}

irc_exception::irc_exception(const IRCCommand& cmd, const std::string& trailing, const std::string& param1, const std::string& param2) throw()
    : _message(HOSTNAME, cmd, trailing)
{
    _message.AddParam(param1);
    _message.AddParam(param2);
}

irc_exception::irc_exception(const IRCCommand& cmd, const std::string& trailing, const std::string& param1, const std::string& param2, const std::string& param3) throw()
    : _message(HOSTNAME, cmd, trailing)
{
    _message.AddParam(param1);
    _message.AddParam(param2);
    _message.AddParam(param3);
}

irc_exception::irc_exception(const IRCCommand& cmd, const std::string& trailing, const std::string& param1, const std::string& param2, const std::string& param3, const std::string& param4) throw()
    : _message(HOSTNAME, cmd, trailing)
{
    _message.AddParam(param1);
    _message.AddParam(param2);
    _message.AddParam(param3);
    _message.AddParam(param4);
}

const IRCMessage& irc_exception::message() const throw()
{
    return _message;
}
