/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircmessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yejsong <yejsong@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 16:43:59 by smun              #+#    #+#             */
/*   Updated: 2022/04/02 14:17:19 by yejsong          ###   ########.fr       */
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

IRCMessage::IRCMessage(const std::string& prefix, const IRCCommand& cmd, const std::string& txt)
    : _prefix(prefix)
    , _cmd(cmd)
    , _params()
{
    _params.push_back(txt);
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

    // Params or Trailing (파라미터 또는 꼬리표. 꼬리표는 공백을 포함하는 문자열을 처리하기 위한 트릭.)
    // while (it != args.end())
    // {
    //     // :로 시작하는 문자열이 나오면, 파라미터가 끝나고, 꼬리표가 나오는 것으로 간주.
    //     if (it->front() == ':')
    //     {
    //         (it++)->substr(1);
    //         break;
    //     }
    //     // :로 시작하지 않았다면, 파라미터를 하나씩 읽어서 추가.
    //     params.push_back(*(it++));
    // }

    // 최종적으로 IRCMessage 인스턴스를 생성.
    IRCMessage msg = IRCMessage(prefix, command);
    // for (it = params.begin(); it != params.end(); ++it)
    //     msg.AddParam(*it);
    for (; it != args.end(); ++it)
        msg.AddParam(*it);

    return msg;

    // [':' <prefix> <SPACE> ] <command> <params> <crlf>
}
