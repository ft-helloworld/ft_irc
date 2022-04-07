/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/06 15:44:33 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 19:02:41 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircbot.hpp"
#include "ircserver.hpp"
#include "ircmessage.hpp"
#include "ircstring.hpp"
#include "irc_exception.hpp"
#include "irccomparer.hpp"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstdlib>

IRCBot::IRCBot(IRCServer* server, const std::string& nickname, const std::string& username)
    : IRCSession(server, NULL, -1, -1, HOSTNAME)
{
    _registerFlag = FLAG_NICKNAME | FLAG_USERNAME;
    SetNickname(nickname);
    SetUsername(username);
    Log::Dp("IRCBot::IRCBot", "새로운 봇 (%s)이 생성되었습니다.", GetMask().c_str());
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
    return ret;
}

void    IRCBot::Send(const void* buf, size_t len)
{
    try
    {
        IRCMessage msg = IRCMessage::Parse(std::string(reinterpret_cast<const char*>(buf), len));
        if (msg.IsEmpty())
            return;
        const std::string& cmd = msg.GetCommand();
        if (cmd != "PRIVMSG" || msg.SizeParam() < 2 || msg.GetPrefix() == GetMask())
            return;
        const std::string& prefix = msg.GetPrefix();
        bool isChannel = msg.GetParam(0) != GetNickname();
        const std::string channel = (isChannel ? msg.GetParam(0) : "");
        OnMessage(prefix.substr(0, prefix.find('!')), channel, msg.GetParams(1));
    }
    catch (const std::exception& ex)
    {
        Log::Ep("IRCBot::Process", "봇 메시지 처리 중 오류가 발생했습니다. (%s)", ex.what());
    }
}

void      IRCBot::SendToChannelOrUser(const std::string& fromNick, const std::string& channel, const std::string& msg)
{
    if (channel.empty())
        SendTo(fromNick, false, msg);
    else
        SendTo(channel, false, msg);
}

void    IRCBot::Close()
{
    Log::Dp("IRCBot::Close", "봇을 Close 하는 요청을 무시했습니다.");
    // do nothing
}

void    IRCBot::CheckActive()
{
    // do nothing
}

static int toLower(int ch)
{
    return std::tolower(ch);
}

void    IRCBot::OnMessage(const std::string& fromNick, const std::string& channel, const std::string& commandline)
{
    std::istringstream iss(commandline);
    std::string line;
    std::vector<std::string> args;

    try
    {
        while (std::getline(iss, line, ' '))
            args.push_back(line);
        if (args.empty())
            throw bot_exception("Not typed command... 명령이 기입되지 않았습니다. 명령을 기입해주세요.");
        std::string cmd = args[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), toLower);

        if (args[0] == "!help")
            OnHelp(fromNick, args);
        else if (args[0] == "!join")
        {
            if (!channel.empty())
                throw bot_exception("해당 명령은 개인 메시지로만 사용 가능한 메시지입니다. /msg "BOTNAME" " + args[0] + " 으로 사용하세요.");
            OnJoin(fromNick, args);
        }
        else if (args[0] == "!msg")
        {
            if (!channel.empty())
                throw bot_exception("해당 명령은 개인 메시지로만 사용 가능한 메시지입니다. /msg "BOTNAME" " + args[0] + " 으로 사용하세요.");
            OnMsg(fromNick, args);
        }
        else if (args[0] == "!rsp")
            OnRockScissorPaper(fromNick, channel, args);
        else if (args[0] == "!dice")
            OnDice(fromNick, channel);
    }
    catch (const bot_exception& err)
    {
        SendTo(fromNick, true, err.message);
    }
    catch (const irc_exception& iex)
    {
        SendTo(fromNick, true, iex.message().GetParams(0));
    }
}

void    IRCBot::OnHelp(const std::string& fromNick, ArgsVector& args)
{
    if (args.size() < 2)
    {
        SendTo(fromNick, true, "안녕하세요 " + fromNick + "! 행복한 하루 되세요 :)");
        SendTo(fromNick, true, "ft_irc에서 사용 가능한 명령어 일람은 아래와 같습니다.");
        SendTo(fromNick, true, "!help, !msg, !join, !rsp, !dice");
    }
    else
    {
        if (args[1] == "?")
            SendTo(fromNick, true, "도움말 명령어 입니다.");
    }
}

void    IRCBot::OnJoin(const std::string& fromNick, ArgsVector& args)
{
    if (args.size() < 2)
        throw bot_exception("[SYNTAX] /msg "BOTNAME" !join <channel>");
    if (IsJoinedChannel(args[1]))
        throw bot_exception("해당 채널 '" + args[1] + "'에 이미 봇이 입장하고 있습니다.");
    _server->JoinChannel(*this, args[1]);
    SendTo(fromNick, true, args[1] + " 채널에 입장합니다.");
}

void    IRCBot::OnMsg(const std::string& fromNick, ArgsVector& args)
{
    (void)fromNick;
    if (args.size() < 3)
        throw bot_exception("[SYNTAX] /msg "BOTNAME" !msg <channel> <msg>");
    IRCChannel* channel = _server->FindChannel(args[1]);
    if (channel == NULL)
        throw bot_exception("Not exists channel '" + args[1] + "'.");
    if (!IsJoinedChannel(args[1]))
        throw bot_exception("This bot is not joined that channel '" + args[1] + "'.");
    channel->Send(IRCMessage(GetMask(), "PRIVMSG", args[1], String::Join(args.begin() + 2, args.end(), " ")));
}

void    IRCBot::OnRockScissorPaper(const std::string& fromNick, const std::string& channel, ArgsVector& args)
{
    static const std::string NAME[] = {"✌️", "✊", "✋"};

    if (args.size() < 2)
    {
        SendToChannelOrUser(fromNick, channel, "[SYNTAX] !rsp <rock(바위)/scissor(가위)/paper(보))>");
        return;
    }

    int user = 0;
    int bot = std::rand() % 3;
    if (args[1] == "scissor" || args[1] == "가위")
        user = 0;
    else if (args[1] == "rock" || args[1] == "바위")
        user = 1;
    else if (args[1] == "paper" || args[1] == "보")
        user = 2;
    else
    {
        SendToChannelOrUser(fromNick, channel, "[SYNTAX] !rsp <rock(바위)/scissor(가위)/paper(보))>");
        return;
    }
    if ((bot + 1) % 3 == user) // 유저 승리
        SendToChannelOrUser(fromNick, channel, "👏👏👏 가위 바위 보에서 이겼습니다! [당신:" + NAME[user] + "] [봇:" + NAME[bot] + "] 👏👏👏");
    else if ((user + 1) % 3 == bot) // 봇 승리
        SendToChannelOrUser(fromNick, channel, "😢😢😢 가위 바위 보에서 졌습니다! [당신:" + NAME[user] + "] [봇:" + NAME[bot] + "] 😢😢😢");
    else // 무승부
        SendToChannelOrUser(fromNick, channel, "🔮🔮🔮 무승부입니다! [당신:" + NAME[user] + "] [봇:" + NAME[bot] + "] 🔮🔮🔮");
}

void    IRCBot::OnDice(const std::string& fromNick, const std::string& channel)
{
    SendToChannelOrUser(fromNick, channel, "주사위 결과 = " + String::ItoString(std::rand() % 100));
}
