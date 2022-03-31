/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_exception.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 19:05:14 by yejsong           #+#    #+#             */
/*   Updated: 2022/03/31 17:09:59 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "irc_exception.hpp"
#include "ircmessage.hpp"
#include <string>

irc_exception::~irc_exception() throw() {}

irc_exception::irc_exception(const irc_exception& o)
    : _message(o._message) {}

irc_exception::irc_exception(int cmd) throw()
    : _message(IRCMessage(HOSTNAME, cmd, "").GetMessage()) {}

irc_exception::irc_exception(int cmd, const std::string& trailing) throw()
    : _message(IRCMessage(HOSTNAME, cmd, trailing).GetMessage()) {}

irc_exception::irc_exception(int cmd, const std::string& trailing, const std::string& param1) throw()
    : _message()
{
    IRCMessage msg(HOSTNAME, cmd, trailing);
    msg.AddParam(param1);
    _message = msg.GetMessage();
}

irc_exception::irc_exception(int cmd, const std::string& trailing, const std::string& param1, const std::string& param2) throw()
    : _message()
{
    IRCMessage msg(HOSTNAME, cmd, trailing);
    msg.AddParam(param1);
    msg.AddParam(param2);
    _message = msg.GetMessage();
}

irc_exception::irc_exception(int cmd, const std::string& trailing, const std::string& param1, const std::string& param2, const std::string& param3) throw()
    : _message()
{
    IRCMessage msg(HOSTNAME, cmd, trailing);
    msg.AddParam(param1);
    msg.AddParam(param2);
    msg.AddParam(param3);
    _message = msg.GetMessage();
}

irc_exception::irc_exception(int cmd, const std::string& trailing, const std::string& param1, const std::string& param2, const std::string& param3, const std::string& param4) throw()
    : _message()
{
    IRCMessage msg(HOSTNAME, cmd, trailing);
    msg.AddParam(param1);
    msg.AddParam(param2);
    msg.AddParam(param3);
    msg.AddParam(param4);
    _message = msg.GetMessage();
}

const char* irc_exception::what() const throw()
{
    return _message.c_str();
}
