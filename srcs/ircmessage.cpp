/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircmessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 16:43:59 by smun              #+#    #+#             */
/*   Updated: 2022/04/06 18:50:33 by smun             ###   ########.fr       */
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
    , _params() {}

IRCMessage::~IRCMessage() {}

IRCMessage::IRCMessage(const IRCMessage& msg)
    : _prefix(msg._prefix)
    , _cmd(msg._cmd)
    , _params(msg._params) {}

IRCMessage::IRCMessage(const IRCCommand& cmd)
    : _prefix()
    , _cmd(cmd)
    , _params() {}

IRCMessage::IRCMessage(const std::string& prefix, const IRCCommand& cmd)
    : _prefix(prefix)
    , _cmd(cmd)
    , _params() {}

IRCMessage::IRCMessage(const std::string& prefix, const IRCCommand& cmd, const std::string& p1)
    : _prefix(prefix)
    , _cmd(cmd)
    , _params()
{
    _params.push_back(p1);
}

IRCMessage::IRCMessage(const std::string& prefix, const IRCCommand& cmd, const std::string& p1, const std::string& p2)
    : _prefix(prefix)
    , _cmd(cmd)
    , _params()
{
    _params.push_back(p1);
    _params.push_back(p2);
}

IRCMessage::IRCMessage(const std::string& prefix, const IRCCommand& cmd, const std::string& p1, const std::string& p2, const std::string& p3)
    : _prefix(prefix)
    , _cmd(cmd)
    , _params()
{
    _params.push_back(p1);
    _params.push_back(p2);
    _params.push_back(p3);
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

    String::SplitArguments(args, line);
    std::vector<const std::string>::iterator it = args.begin();
    if (it == args.end())
        return Empty;

    // Prefix (맨 처음, :로 시작 하는 문자열. 선택이기에 없을 수도 있음.)
    if (it->front() == ':')
    {
        prefix = it->substr(1);
        ++it;
    }

    // Command (명령 단어. 항상 대문자로 처리한다.)
    command = *(it++);
    std::transform(command.begin(), command.end(), command.begin(), toUpper);

    // 최종적으로 IRCMessage 인스턴스를 생성.
    IRCMessage msg = IRCMessage(prefix, command);
    // for (it = params.begin(); it != params.end(); ++it)
    //     msg.AddParam(*it);
    for (; it != args.end(); ++it)
        msg.AddParam(*it);

    return msg;

    // [':' <prefix> <SPACE> ] <command> <params> <crlf>
}

const std::string  IRCMessage::GetParams(ParamVector::size_type begin, ParamVector::size_type end) const
{
    const ParamVector::const_iterator beginPos = _params.begin() + begin;
    const ParamVector::const_iterator endPos = _params.begin() + end;

    std::ostringstream oss;
    ParamVector::const_iterator it;
    for (it = beginPos; it != endPos && it != _params.end(); ++it)
    {
        if (it != beginPos)
            oss << " ";
        oss << *it;
    }
    return oss.str();
}
