/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/06 15:44:33 by smun              #+#    #+#             */
/*   Updated: 2022/04/06 16:23:31 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircbot.hpp"
#include "ircserver.hpp"
#include "ircmessage.hpp"
#include "ircstring.hpp"

IRCBot::IRCBot(IRCServer* server, const std::string& nickname, const std::string& username)
    : IRCSession(server, NULL, -1, -1, HOSTNAME)
{
    _registerFlag = FLAG_NICKNAME | FLAG_USERNAME;
    SetNickname(nickname);
    SetUsername(username);
    Log::Ip("IRCBot::IRCBot", "새로운 봇 (%s)이 생성되었습니다.", GetMask().c_str());
}

IRCBot::~IRCBot()
{
    Log::Dp("IRCBot::~IRCBot", "봇 %s 인스턴스가 삭제됩니다.", GetMask().c_str());
}

size_t    IRCBot::SendTo(const std::string& param, bool notice, const std::string& msg)
{
    const char* cmd = notice ? "NOTICE" : "PRIVMSG";

    size_t ret = 0;
    if (param.empty())
        return ret;
    IRCString::TargetSet targets;
    IRCString::SplitTargets(targets, param);
    for (IRCString::TargetSet::const_iterator it = targets.begin(); it != targets.end(); ++it)
    {
        const std::string& target = *it;
        if (IRCString::IsChannelPrefix(target.front()))
        {
            IRCChannel* channel = _server->FindChannel(target);
            if (channel == NULL)
            {
                Log::Vp("IRCBot::SendTo", "봇이 메시지를 전송하려 했으나, 대상 채널 [%s]을 발견할 수 없었습니다.", target.c_str());
                continue;
            }
            channel->Send(IRCMessage(GetMask(), cmd, target, msg), this);
        }
        else
        {
            IRCSession* session = _server->FindByNick(target);
            if (session == NULL)
            {
                Log::Vp("IRCBot::SendTo", "봇이 메시지를 전송하려 했으나, 대상 [%s]을 발견할 수 없었습니다.", target.c_str());
                continue;
            }
            session->SendMessage(IRCMessage(GetMask(), cmd, target, msg));
        }
        ++ret;
    }
}

void    IRCBot::Send(const void* buf, size_t len);
{
    try
    {
        IRCMessage msg = IRCMessage::Parse(line);
        if (msg.IsEmpty())
            return;
        const std::string& cmd = msg.GetCommand();
        if (cmd != "PRIVMSG")
            return;
        OnMessage(msg.GetPrefix(), msg);
    }
    catch (const std::exception& ex)
    {
        Log::Ep("IRCBot::Process", "봇 메시지 처리 중 오류가 발생했습니다. (%s)", ex.what());
    }
}

void    IRCBot::Close()
{
    // ...
}

void    IRCBot::CheckActive()
{
    // do nothing
}
