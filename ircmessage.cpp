/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircmessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 16:43:59 by smun              #+#    #+#             */
/*   Updated: 2022/04/01 15:37:47 by smun             ###   ########.fr       */
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
    , _trailing(msg._trailing){}

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
    while (it != args.end())
    {
        // :로 시작하는 문자열이 나오면, 파라미터가 끝나고, 꼬리표가 나오는 것으로 간주.
        if (it->front() == ':')
        {
            std::ostringstream oss;

            oss << (it++)->substr(1);
            while (it != args.end())
                oss << " " << *(it++);
            trailing = oss.str();
            break;
        }

        // :로 시작하지 않았다면, 파라미터를 하나씩 읽어서 추가.
        params.push_back(*(it++));
    }

    // 최종적으로 IRCMessage 인스턴스를 생성.
    IRCMessage msg = IRCMessage(prefix, command, trailing);
    for (it = params.begin(); it != params.end(); ++it)
        msg.AddParam(*it);

    return msg;

    // [':' <prefix> <SPACE> ] <command> <params> <crlf>
}
