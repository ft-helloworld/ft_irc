/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircmessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 16:43:59 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 22:05:15 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircmessage.hpp"
#include <sstream>
#include <algorithm>
#include <string>

const IRCMessage IRCMessage::Empty = IRCMessage();

IRCMessage::IRCMessage()
    : _prefix()
    , _cmd(0)
    , _params()
    , _trailing() {}

IRCMessage::~IRCMessage() {}

IRCMessage::IRCMessage(const IRCMessage& msg)
    : _prefix(msg._prefix)
    , _cmd(msg._cmd)
    , _params(msg._params)
    , _trailing(msg._trailing) {}

IRCMessage::IRCMessage(const IRCCommand& cmd)
    : _prefix()
    , _cmd(cmd)
    , _params()
    , _trailing() {}

IRCMessage::IRCMessage(const IRCCommand& cmd, const std::string& trailing)
    : _prefix()
    , _cmd(cmd)
    , _params()
    , _trailing(trailing) {}

IRCMessage::IRCMessage(const std::string& prefix, const IRCCommand& cmd, const std::string& trailing)
    : _prefix(prefix)
    , _cmd(cmd)
    , _params()
    , _trailing(trailing) {}

void    IRCMessage::AddParam(const std::string& param)
{
    _params.push_back(param);
}

IRCMessage::ParamVectorConstIterator    IRCMessage::BeginParam() const { return _params.begin(); }
IRCMessage::ParamVectorConstIterator    IRCMessage::EndParam() const { return _params.end(); }
const std::string&                      IRCMessage::GetParam(ParamVector::size_type i) const { return _params[i]; }
IRCMessage::ParamVector::size_type      IRCMessage::SizeParam() const { return _params.size(); }

const std::string&  IRCMessage::GetCommand() const { return _cmd.command; }
const std::string&  IRCMessage::GetTrailing() const { return _trailing; }
const std::string&  IRCMessage::GetPrefix() const { return _prefix; }

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
        oss << " :" << _trailing;

    return oss.str();
}

static int toUpper(char ch)
{
    return std::toupper(ch);
}

IRCMessage IRCMessage::Parse(const std::string& line)
{
    std::vector<const std::string> args;
    std::vector<const std::string> params;
    std::string prefix;
    std::string command;
    std::string trailing;

    String::SplitArguments(args, line);
    std::vector<const std::string>::const_iterator it = args.begin();
    if (it == args.end())
        return Empty;

    // Prefix
    if (it->front() == ':')
    {
        prefix = it->substr(1);
        ++it;
    }

    // Command
    command = *(it++);
    std::transform(command.begin(), command.end(), command.begin(), toUpper);


    // Params or Trailing
    while (it != args.end())
    {
        // Trailing and break loop
        if (it->front() == ':')
        {
            trailing = it->substr(1);
            break;
        }

        // Param
        params.push_back(*(it++));
    }

    // Make IRCMessage
    IRCMessage msg = IRCMessage(prefix, command, trailing);
    for (it = params.begin(); it != params.end(); ++it)
        msg.AddParam(*it);

    return msg;

    // [':' <prefix> <SPACE> ] <command> <params> <crlf>
    // Parse prefix
}

bool    IRCMessage::IsEmpty() const
{
    return _cmd.command.empty();
}
