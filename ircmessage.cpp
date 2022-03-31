/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircmessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 16:43:59 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 18:40:26 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircmessage.hpp"
#include <sstream>

IRCMessage::~IRCMessage() {}

IRCMessage::IRCMessage(const IRCMessage& msg)
    : _prefix(msg._prefix)
    , _cmd(msg._cmd)
    , _trailing(msg._trailing) {}

IRCMessage::IRCMessage(const IRCCommand& cmd)
    : _prefix()
    , _cmd(cmd)
    , _trailing() {}

IRCMessage::IRCMessage(const IRCCommand& cmd, const std::string& trailing)
    : _prefix()
    , _cmd(cmd)
    , _trailing(trailing) {}

IRCMessage::IRCMessage(const std::string& prefix, const IRCCommand& cmd, const std::string& trailing)
    : _prefix(prefix)
    , _cmd(cmd)
    , _trailing(trailing) {}

void    IRCMessage::AddParam(const std::string& param)
{
    _params.push_back(param);
}

const std::string IRCMessage::GetMessage() const
{
    std::ostringstream oss;

    // :prefix
    if (!_prefix.empty())
        oss << ":" << _prefix << " ";

    // cmd
    oss << _cmd.command;

    // params
    ParamVectorConstIterator it;
    for (it = _params.begin(); it != _params.end(); ++it)
        oss << " " << *it;

    // :trailing
    if (!_trailing.empty())
        oss << ":" << _trailing;

    oss << CRLF;

    return oss.str();
}
