/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 19:34:57 by yejsong           #+#    #+#             */
/*   Updated: 2022/04/02 19:28:31 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircserver.hpp"
#include "irc_exception.hpp"
#include "ircsession.hpp"
#include "ircstring.hpp"
#include "ircmessage.hpp"
#include "ircnumericmessage.hpp"
#include <sstream>
#include <string>
#include <stdexcept>
#include <set>

IRCServer::IRCServer(const std::string& password)
    : _password(password)
    , _clients()
    , _channels() {}

IRCServer::~IRCServer() {}

IRCSession* IRCServer::FindByNick(const std::string& nick) const
{
    ClientMap::const_iterator it = _clients.find(nick);
    if (it == _clients.end())
        return 0;
    return it->second;
}

void    IRCServer::OnNickname(IRCSession& session, IRCMessage& msg)
{
    // 메시지에 파라미터가 없었다면 에러
    if (msg.SizeParam() == 0)
        throw irc_exception(ERR_NONICKNAMEGIVEN, "NICK", "No nickname given");

    const std::string& nick = msg.GetParam(0);

    // 닉네임이 유효한 문자열 조합이 아니라면 에러
    if (!IRCString::IsValidNick(nick))
        throw irc_exception(ERR_ERRONEUSNICKNAME, nick, "Erroneous nickname");

    if (nick == session.GetNickname())
        return;

    // 서버에 이미 등록된 닉네임이라면 에러
    if (_clients.find(nick) != _clients.end())
        throw irc_exception(ERR_NICKNAMEINUSE, nick, "Nickname is already in use", nick);

    // 닉네임 변경 알림을 같은 채널에 입장한 사람들 및 자기 자신에게 전송
    if (session.IsFullyRegistered())
    {
        const IRCMessage nickmsg(session.GetMask(), "NICK", nick);
        session.SendMessageToNeighbor(nickmsg, &session);
        session.SendMessage(nickmsg);
    }

    // 기존의 닉네임 삭제
    _clients.erase(session.GetNickname());
    Log::Vp("IRCServer::CheckNickname", "닉네임 '%s' 가 서버에서 삭제되었습니다.", session.GetNickname().c_str());

    // 새로운 닉네임 등록
    _clients[nick] = &session;
    Log::Vp("IRCServer::CheckNickname", "닉네임 '%s' 가 서버에 등록되었습니다.", nick.c_str());

    // 세션에도 닉네임 설정
    session.SetNickname(nick);

    // 세션에 닉네임이 등록되었다는 플래그 설정
    Log::Vp("IRCServer::CheckNickname", "현재 세션에서 FLAG_NICKNAME 플래그가 설정되었습니다.");
    session.RegisterStep(IRCSession::FLAG_NICKNAME);
}

void    IRCServer::OnUsername(IRCSession& session, IRCMessage& msg)
{
    // USER 커맨드는 총 4개의 파라미터 필요. 부족하면 에러.
    if (msg.SizeParam() < 4)
        throw irc_exception(ERR_NEEDMOREPARAMS, "USER", "Not enough parameters");

    // 이미 USER, NICK 모두 등록되었다면 에러
    if (session.IsFullyRegistered())
        throw irc_exception(ERR_ALREADYREGISTRED, "USER", "You may not reregister");

    const std::string& username = msg.GetParam(0);

    // 세션에도 유저네임 설정
    session.SetUsername(username);

    // 세션에 유저네임이 등록되었다는 플래그 설정
    Log::Vp("IRCServer::CheckNickname", "현재 세션에서 FLAG_USERNAME 플래그가 설정되었습니다.");
    session.RegisterStep(IRCSession::FLAG_USERNAME);
}

void    IRCServer::OnPassword(IRCSession& session, IRCMessage& msg)
{
    // PASS 커맨드에 파라미터가 없었다면 에러
    if (msg.SizeParam() == 0)
        throw irc_exception(ERR_NEEDMOREPARAMS, "PASS", "Not enough parameters");

    // 이미 USER, NICK 모두 등록되었다면 에러
    if (session.IsFullyRegistered())
        throw irc_exception(ERR_ALREADYREGISTRED, "You may not reregister");

    // 세션에 입력받은 접속 패스워드 설정
    session.SetPassword(msg.GetParam(0));
}

void    IRCServer::OnQuit(IRCSession& session, IRCMessage& msg)
{
    // 꼬리표에서 접속 종료 사유를 클라이언트로부터 얻음.
    std::string quitReason = msg.GetParam(0);
    if (quitReason.empty())
        quitReason = "접속 종료";

    // 해당 종료 사유를 이용해 접속 종료
    session.Close("Quit: " + quitReason);
}

void    IRCServer::OnJoin(IRCSession& session, IRCMessage& msg)
{
    // ERR_NEEDMOREPARAMS 파라미터 부족
    if (msg.SizeParam() < 1 || msg.GetParam(0).empty())
        throw irc_exception(ERR_NEEDMOREPARAMS, "JOIN", "Not enough parameters");
    // ERR_BANNEDFROMCHAN 구현 필요 없음
    // ERR_INVITEONLYCHAN 구현 필요 없음
    // ERR_BADCHANNELKEY 구현 필요 없음
    // ERR_CHANNELISFULL 구현 필요 없음

    IRCString::TargetSet targets;
    IRCString::TargetSet::const_iterator sit;

    IRCString::SplitTargets(targets, msg.GetParam(0));
    for (sit = targets.begin(); sit != targets.end(); ++sit)
    {
        const std::string& chanName = *sit;

        // ERR_BADCHANMASK 채널명이 #, &로 시작하지 않을 때
        if (chanName.front() != '#' && chanName.front() != '&')
        {
            session.SendMessage(IRCNumericMessage(ERR_BADCHANMASK, chanName, "Invalid channel name"));
            continue;
        }
        // ERR_NOSUCHCHANNEL 채널명 규칙 잘못됨 (길이 초과/문자열 잘못됨)
        if (!IRCString::IsValidChstring(chanName.substr(1)))
        {
            session.SendMessage(IRCNumericMessage(ERR_NOSUCHCHANNEL, chanName, "No such channel"));
            continue;
        }
        // ERR_TOOMANYCHANNELS 더 이상 들어갈 수 없을 때. (대충 최대 개수 지정)
        if (session.GetJoinedChannelNum() >= MAX_CHANNEL)
        {
            session.SendMessage(IRCNumericMessage(ERR_TOOMANYCHANNELS, chanName, "You have joined too many channels"));
            continue;
        }
        // RPL_TOPIC 구현 필요 없음

        // 이미 채널에 참여중
        if (session.IsJoinedChannel(chanName))
        {
            Log::Vp("IRCServer::OnJoin", "유저 <%s>는 이미 채널 '%s'에 참여중입니다. JOIN 요청을 무시합니다.", session.GetEmail().c_str(), chanName.c_str());
            continue;
        }

        // 1. 채널이 없으면 생성
        ChannelMap::iterator chanIt = _channels.find(chanName);
        IRCChannel* chan;
        bool    newChan = false;
        if (chanIt == _channels.end())
        {
            typedef std::pair<const std::string, SharedPtr<IRCChannel> > ChannelPair;
            _channels.insert(ChannelPair(chanName, SharedPtr<IRCChannel>(chan = new IRCChannel(chanName))));
            newChan = true;
            Log::Vp("IRCServer::OnJoin", "새로운 채널 '%s'가 유저 <%s> 에 의해 생성됩니다.", chanName.c_str(), session.GetEmail().c_str());
        }
        else
            chan = chanIt->second.Load();

        // 2. 채널에 입장
        session.AddChannel(chan->GetChannelName());

        chan->Join(session);
        Log::Vp("IRCServer::OnJoin", "유저 <%s> 가 채널 '%s'에 참여합니다.", session.GetEmail().c_str(), chanName.c_str());

        // 3. 채널에 Names 커맨드 사용
        chan->SendNames(session);
    }
}

void    IRCServer::OnPart(IRCSession& session, IRCMessage& msg)
{
    // ERR_NEEDMOREPARAMS 파라미터 부족
    if (msg.SizeParam() < 1 || msg.GetParam(0).empty())
        throw irc_exception(ERR_NEEDMOREPARAMS, "PART", "Not enough parameters");

    IRCString::TargetSet targets;
    IRCString::TargetSet::const_iterator sit;

    IRCString::SplitTargets(targets, msg.GetParam(0));
    for (sit = targets.begin(); sit != targets.end(); ++sit)
    {
        // ERR_NOSUCHCHANNEL 채널명 규칙 잘못됨 (길이 초과/문자열 잘못됨)
        const std::string& chanName = *sit;

        if (!IRCString::IsChannelPrefix(chanName.front()))
        {
            session.SendMessage(IRCNumericMessage(ERR_NOSUCHCHANNEL, chanName, "Invalid channel name"));
            continue;
        }
        if (!IRCString::IsValidChstring(chanName.substr(1)))
        {
            session.SendMessage(IRCNumericMessage(ERR_NOSUCHCHANNEL, chanName, "No such channel"));
            continue;
        }
        LeaveChannel(session, chanName, "PART");
    }
}

void    IRCServer::LeaveChannel(IRCSession& session, const std::string& chanName, const std::string& cmd)
{
    ChannelMap::iterator chanIt = _channels.find(chanName);
    if (chanIt == _channels.end())
        throw irc_exception(ERR_NOSUCHCHANNEL, chanName, "No such channel");

    // ERR_NOTONCHANNEL 나갈 채널에 없을 때
    if (!session.RemoveChannel(chanName))
        throw irc_exception(ERR_NOTONCHANNEL, chanName, "You're not on that channel");

    // 1. 채널이 있으면 거기서 퇴장
    chanIt->second.Load()->Part(session, cmd);
    Log::Vp("IRCServer::LeaveChannel", "유저 <%s> 가 채널 '%s'에서 %s 명령으로 퇴장합니다.", session.GetEmail().c_str(), chanName.c_str(), cmd.c_str());

    CheckChannelExpire(chanIt->second.Load());
}

void    IRCServer::OnNames(IRCSession& session, IRCMessage& msg)
{
    // RPL_NAMREPLY
    // RPL_ENDOFNAMES

    // 1. 조건에 맞으면 채널의 이름들 전송.
    // 따로 오류 응답은 없음.

    if (msg.SizeParam() == 0 || msg.GetParam(0) == "*")
    {
        ChannelMap::const_iterator it;
        for (it = _channels.begin(); it != _channels.end(); ++it)
        {
            const IRCChannel* chan = it->second.Load();
            if (chan->HasFlag(IRCChannel::MODE_PRIV) || chan->HasFlag(IRCChannel::MODE_SECRET))
                continue;
            chan->SendNames(session, false);
        }
        session.SendMessage(IRCNumericMessage(RPL_ENDOFNAMES, "*", "End of /NAMES list"));
    }
    else
    {
        IRCString::TargetSet targets;
        IRCString::TargetSet::const_iterator sit;

        IRCString::SplitTargets(targets, msg.GetParam(0));
        for (sit = targets.begin(); sit != targets.end(); ++sit)
        {
            ChannelMap::const_iterator it = _channels.find(*sit);
            if (it == _channels.end())
                session.SendMessage(IRCNumericMessage(ERR_NOSUCHNICK, *sit, "No such nick/channel"));
            else
                it->second.Load()->SendNames(session);
        }
    }
}

void    IRCServer::OnPrivMsg(IRCSession& session, IRCMessage& msg, const std::string& cmd)
{
    // ERR_NORECIPIENT 파라미터 없을 때
    if (msg.SizeParam() < 1)
        throw irc_exception(ERR_NORECIPIENT, cmd, "No recipient given");
    // ERR_NOTEXTTOSEND 보낼 메시지 없을 때
    if (msg.SizeParam() < 2)
        throw irc_exception(ERR_NOTEXTTOSEND, cmd, "No text to send");

    if (msg.GetParam(0).empty())
        throw irc_exception(ERR_NORECIPIENT, cmd, "No recipient given");
    const std::string& message = msg.GetParams(1);
    if (message.empty())
        throw irc_exception(ERR_NOTEXTTOSEND, cmd, "No text to send");

    // ERR_CANNOTSENDTOCHAN +n(noextmsg) 플래그가 채널에 있고, 보낸 유저가 채널에 없을 경우
    //                      +m(moderated) 플래그가 채널에 있고, 보낸 유저가 chanop가 아니고, +v 모드가 없을 경우
    // ERR_NOTOPLEVEL 구현 필요 없음
    // ERR_WILDTOPLEVEL 구현 필요 없음
    // ERR_TOOMANYTARGETS 구현 필요 없음
    // ERR_NOSUCHNICK 해당 채널 또는 닉네임이 없을 때

    IRCString::TargetSet targets;
    IRCString::TargetSet::const_iterator sit;

    IRCString::SplitTargets(targets, msg.GetParam(0));
    for (sit = targets.begin(); sit != targets.end(); ++sit)
    {
        const std::string& recipient = *sit;

        if (IRCString::IsChannelPrefix(recipient.front()))
        {
            ChannelMap::const_iterator it = _channels.find(recipient);
            if (it == _channels.end())
            {
                session.SendMessage(IRCNumericMessage(ERR_NOSUCHNICK, recipient, "No such channel"));
                continue;
            }
            IRCMessage sendmsg(session.GetMask(), cmd, recipient, message);
            it->second.Load()->Send(sendmsg, &session);
            Log::Vp("IRCServer::OnPrivMsg", "유저 <%s>가 채널 '%s'에 %llu 바이트의 메시지를 보냈습니다.", session.GetEmail().c_str(), recipient.c_str(), message.size());
        }
        else
        {
            // RPL_AWAY 잠수 메시지 보내 주는 기능. 구현 필요 없음
            IRCSession* target = FindByNick(recipient);
            if (target == NULL)
            {
                session.SendMessage(IRCNumericMessage(ERR_NOSUCHNICK, recipient, "No such user"));
                continue;
            }
            IRCMessage sendmsg(session.GetMask(), cmd, recipient, message);
            target->SendMessage(sendmsg);
            Log::Vp("IRCServer::OnPrivMsg", "유저 <%s>가 대상 '%s'에 %llu 바이트의 메시지를 보냈습니다.", session.GetEmail().c_str(), recipient.c_str(), message.size());
        }
    }
}

void    IRCServer::UnregisterNickname(const std::string& nick)
{
    _clients.erase(nick);
    Log::Vp("IRCServer::UnregisterNickname", "닉네임 '%s' 가 서버에서 삭제되었습니다.", nick.c_str());
}

bool IRCServer::IsPasswordMatched(const std::string& password) const
{
    return _password == password;
}

void    IRCServer::CheckChannelExpire(IRCChannel* channel)
{
    if (!channel->IsEmpty())
        return;

    Log::Vp("IRCServer::CheckChannelExpire", "채널 '%s'가 비었습니다. 채널을 삭제합니다.", channel->GetChannelName().c_str());
    _channels.erase(channel->GetChannelName());
}
