/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsession.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 15:03:56 by smun              #+#    #+#             */
/*   Updated: 2022/03/30 15:13:31 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

IRCSession::IRCSession(Channel* channel, int socketfd, int socketId, const std::string& addr)
    : Session(channel, socketfd, socketId, addr) {}

IRCSession::~IRCSession() {}

void IRCSession::Process(const std::string& line)
{
    // 부모 클래스의 Process 먼저 실행
    Session::Process(line);

    // 날아온 한 줄 처리
    std::vector<const std::string> args;

    // 한 줄에서 스페이스 문자로 구분
    String::SplitArguments(args, line);
    if (args.size() == 0)
        return;

    // 명령어 처리
    try
    {
        if (args[0] == "HELLO")
        {
            if (args.size() < 2)
                throw std::runtime_error("No parameter with HELLO.");
            Send("HELLO Your name is " + args[1] + "!! Welcome to my server :)");
        }
        else if (args[0] == "MESSAGE")
        {
            if (args.size() < 3)
                throw std::runtime_error("Lacked parameter with MESSAGE.");
            int targetId = String::Stoi(args[1]);
            std::string message = String::Join(args.begin() + 2, args.end());
            Session& target = _attachedChannel->FindSession(targetId);
            target.Send("MESSAGE " + message);
        }
        else
            Send("Unknown command " + args[0]);
    }
    catch (const std::exception& ex)
    {
        Send(ex.what());
    }
}
