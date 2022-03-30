/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsession.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 15:03:56 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 01:53:33 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircserver.hpp"
#include "ircsession.hpp"
#include "irc_exception.hpp"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <map>

IRCSession::IRCSession(IRCServer* server, Channel* channel, int socketfd, int socketId, const std::string& addr)
    : Session(channel, socketfd, socketId, addr)
    , _nickname()
    , _username()
    , _server(server)
    {}

IRCSession::~IRCSession() {}

static int toUpper(char ch)
{
    return std::toupper(ch);
}

void IRCSession::Process(const std::string& line)
{
    // 부모 클래스의 Process 먼저 실행
    Session::Process(line);

    // 날아온 한 줄 처리

    // 한 줄에서 스페이스 문자로 구분
    std::vector<const std::string> args;
    String::SplitArguments(args, line);

    // 빈 명령줄이라면 아무 것도 안함.
    if (args.size() == 0)
        return;

    // 명령어 처리
    try
    {
        std::string cmd = args[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), toUpper);
        if (cmd == "NICK")
            _server->CheckNickname(*this, GetNickname(), args[1]);
        else if (cmd == "USER")
        {
            if (args.size() < 5)
                throw irc_exception(ERR_NEEDMOREPARAMS, cmd + " :Not enough parameters");
            const std::string& username = args[1];
            if (!GetUsername().empty())
                throw irc_exception(ERR_ALREADYREGISTRED, ":Unauthorized command (already registered)");
            SetUsername(username);
            Reply(RPL_WELCOME, username + " :Welcome to the Internet Relay Network " + GetNickname() + "!" + username + "@" + GetRemoteAddress());
        }
        else // :bassoon.irc.ozinger.org 421 smun NNNNDD :Unknown command
        {
            throw irc_exception(ERR_UNKNOWNCOMMAND, GetNickname() + " " + cmd + " :Unknown command");
        }

    }
    catch (const std::exception& ex)
    {
        Send(ex.what());
    }
}

void    IRCSession::Reply(int statuscode, const std::string& line)
{
    Send(":"HOSTNAME" " + String::ItoCode(statuscode) + " " + line);
}

void    IRCSession::SetNickname(const std::string& nickname) { _nickname = nickname; }
void    IRCSession::SetUsername(const std::string& username) { _username = username; }

const std::string&  IRCSession::GetNickname() const { return _nickname; }
const std::string&  IRCSession::GetUsername() const { return _username; }
