/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsession.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yejsong <yejsong@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 15:03:56 by smun              #+#    #+#             */
/*   Updated: 2022/03/30 20:39:40 by yejsong          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

IRCSession::IRCSession(IRCServer* server, Channel* channel, int socketfd, int socketId, const std::string& addr)
    : Session(channel, socketfd, socketId, addr)
    , _nickname()
    , _username()
    , _server(server)
    {}

IRCSession::~IRCSession() {}

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
        if (args[0] == "NICK")
        {
            _server->CheckNickname(*this, GetNickname(), args[1]);
            throw irc_exception("433", args[1] + " :Nickname is already in use");
        }
        else if (args[0] == "USER")
        {
            if (args.size() < 5)
                throw irc_exception("461", args[0] + " :Not enough parameters");
            const std::string& username = args[1];
            if (!GetUsername().empty())
                throw irc_exception("462", ":Unauthorized command (already registered)");
            SetUsername(username);
            Reply("001", username + " :Welcome to the Internet Relay Network " + GetNickname() + "!" + username + "@" + GetRemoteAddress());
        }
        else
            Send("Unknown command " + args[0]);
    }
    catch (const std::exception& ex)
    {
        Send(ex.what());
    }
}

void    IRCSession::Reply(const std::string& statuscode, const std::string& line)
{
    Send(std::string(":") + HOSTNAME + " " + statuscode + " " + line);
}

void    IRCSession::SetNickname(const std::string& nickname) { _nickname = nickname; }
void    IRCSession::SetUsername(const std::string& username) { _username = username; }

const std::string&  IRCSession::GetNickname() const { return _nickname; }
const std::string&  IRCSession::GetUsername() const { return _username; }
