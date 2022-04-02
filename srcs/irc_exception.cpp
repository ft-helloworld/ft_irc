/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_exception.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 19:05:14 by yejsong           #+#    #+#             */
/*   Updated: 2022/04/02 15:55:22 by smun             ###   ########.fr       */
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
    : _message(HOSTNAME, cmd)
{
}

irc_exception::irc_exception(const IRCCommand& cmd, const std::string& p1) throw()
    : _message(HOSTNAME, cmd, p1)
{
}

irc_exception::irc_exception(const IRCCommand& cmd, const std::string& p1, const std::string& p2) throw()
    : _message(HOSTNAME, cmd, p1, p2)
{
}

irc_exception::irc_exception(const IRCCommand& cmd, const std::string& p1, const std::string& p2, const std::string& p3) throw()
    : _message(HOSTNAME, cmd, p1, p2, p3)
{
    _message.AddParam(p3);
}

irc_exception::irc_exception(const IRCCommand& cmd, const std::string& p1, const std::string& p2, const std::string& p3, const std::string& p4) throw()
    : _message(HOSTNAME, cmd, p1, p2, p3)
{
    _message.AddParam(p4);
}

irc_exception::irc_exception(const IRCCommand& cmd, const std::string& p1, const std::string& p2, const std::string& p3, const std::string& p4, const std::string& p5) throw()
    : _message(HOSTNAME, cmd, p1, p2, p3)
{
    _message.AddParam(p4);
    _message.AddParam(p5);
}

const IRCMessage& irc_exception::message() const throw()
{
    return _message;
}
