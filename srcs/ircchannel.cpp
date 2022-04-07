/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircchannel.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 16:00:52 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 22:26:39 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircchannel.hpp"
#include "ircnumericmessage.hpp"
#include "ircsession.hpp"
#include "numerics.hpp"
#include "ircserver.hpp"
#include "log.hpp"
#include <string>
#include <sstream>
#include <iterator>
#include <vector>

IRCChannel::IRCChannel(const std::string& name)
    : _name(name)
    , _created(std::time(NULL))
    , _topic()
    , _set_topic()
    , _mask_topic()
    , _participants()
    , _flags(0)
{
}

IRCChannel::~IRCChannel()
{
}

void    IRCChannel::SendNames(IRCSession& session, bool sendEndReply) const
{
    // :c4r9s1.42seoul.kr 353 abcd @ #42Seoul :abcd @smun
    // :c4r9s1.42seoul.kr 366 abcd #42Seoul :End of /NAMES list.
    // 353     RPL_NAMREPLY
    //                 "<channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
    // 366     RPL_ENDOFNAMES
    //                 "<channel> :End of /NAMES list"

    std::ostringstream oss;

    // 참여자가 있을 경우
    if (!_participants.empty())
    {
        std::vector<const std::string> names;

        ParticipantMap::const_iterator  it;
        for (it = _participants.begin(); it != _participants.end(); ++it)
        {
            IRCSession* session = it->first;
            if (it->second & IRCChannel::MODE_OP)
                names.push_back("@" + session->GetNickname());
            else
                names.push_back(session->GetNickname());
        }
        std::copy(names.begin(), names.end(), std::ostream_iterator<const std::string>(oss, " "));

        // 아래 플래그 출력은 RFC1459 문서에는 기재되어 있지 않으나,
        // LimeChat 클라이언트가 RFC2812 표준의 NAMES 명령어 처리를 하므로, 이에 대응.
    }

    char chanFlag = '=';
    if (HasFlag(MODE_PRIV))
        chanFlag = '*';
    else if (HasFlag(MODE_SECRET))
        chanFlag = '@';

    if (oss.tellp() != std::streampos(0))
        session.SendMessage(IRCNumericMessage(RPL_NAMREPLY, std::string(1, chanFlag), _name, oss.str()));

    // 종료 응답 전송
    if (sendEndReply)
        session.SendMessage(IRCNumericMessage(RPL_ENDOFNAMES, _name, "End of /NAMES list"));
}

void    IRCChannel::Join(IRCSession& session)
{
    // :abcd!abcd@0::1 JOIN :#42
    // :c4r9s1.42seoul.kr 353 abcd @ #42 :smun abcd
    // :c4r9s1.42seoul.kr 366 abcd #42 :End of /NAMES list.

    // 방에 아무도 없었을 경우, 방장 부여
    if (_participants.empty())
        _participants[&session] = IRCChannel::MODE_OP;
    else
        _participants[&session] = 0;
    Log::Vp("IRCChannel::Part", "%s 유저가 %s 채널에 입장합니다.", session.GetMask().c_str(), GetChannelName().c_str());
    Send(IRCMessage(session.GetMask(), "JOIN", GetChannelName()));
}

void    IRCChannel::Part(IRCSession& session, const std::string& cmd)
{
    Log::Vp("IRCChannel::Part", "%s 유저가 %s 채널에서 퇴장합니다.", session.GetMask().c_str(), GetChannelName().c_str());
    if (cmd == "PART")
        Send(IRCMessage(session.GetMask(), cmd, GetChannelName()));
    _participants.erase(&session);
}

void    IRCChannel::SendMode(IRCSession& session)
{
    std::string c_mode;
    MakeChannelModeString(c_mode, false);
    session.SendMessage(IRCNumericMessage(RPL_CHANNELMODEIS, GetChannelName(), c_mode));
    session.SendMessage(IRCNumericMessage(RPL_CREATIONTIME, GetChannelName(), String::ItoString(GetCreatedTime())));
}

void    IRCChannel::Send(const IRCMessage& msg, IRCSession* except) const
{
    ParticipantMap::const_iterator  it;

    Log::Vp("IRCChannel::Send", "채널 '%s'에서 IRC메시지 [%s]를 %llu 명에게 전송합니다.", GetChannelName().c_str(),msg.GetCommand().c_str(), _participants.size());
    size_t i = 0;
    for (it = _participants.begin(); it != _participants.end(); ++it)
    {
        if (except != it->first)
        {
            it->first->SendMessage(msg);
            ++i;
        }
    }
    Log::Vp("IRCChannel::Send", "채널 '%s'에서 IRC메시지 [%s]가 %llu 명에게 전송되었습니다.", GetChannelName().c_str(), msg.GetCommand().c_str(), i);
}

void    IRCChannel::GatherParticipants(std::set<IRCSession*>& targets, IRCSession* except)
{
    ParticipantMap::const_iterator  it;

    for (it = _participants.begin(); it != _participants.end(); ++it)
        if (except != it->first)
            targets.insert(it->first);
}

void    IRCChannel::SetChannelTopic(const std::string& topic, const time_t time, const std::string& mask)
{
    _topic = topic;
    _set_topic = time;
    _mask_topic = mask;
}

void     IRCChannel::MakeChannelModeString(std::string& ret, bool val)
{
    if (val == true)
        ret = "[+";
    else
        ret = "+";
    if (_flags & MODE_PRIV)
        ret += "p";
    if (_flags & MODE_SECRET)
        ret += "s";
    if (_flags & MODE_OUTSIDE)
        ret += "n";
    if (val == true)
        ret += "]";
}

std::string&    IRCChannel::RetrunChannelModeString(IRCSession& session, std::string& tmp, std::string& res)
{
    size_t  i = 0;
    if (tmp[i] == '+' || tmp[i] == '-')
        res = tmp[i++];
    while (i < tmp.size())
    {
        int c = tmp[i];
        if (c == '+' || c == '-' || c == 'o' || c == 'p' || c == 's' || c == 'n')
            res += c;
        else
            session.SendMessage(IRCNumericMessage(ERR_UNKNOWNMODE, std::string(1, c), "is unknown mode char to me")); //수정
        i++;
    }
    return (res);
}

int        IRCChannel::SetChannelMode(IRCServer* server, const ModeChange& modeChange)
{
    const bool adding = modeChange.sign == '+';

    int modeFlag = 0;
    if (modeChange.ch == 'o')
        modeFlag = MODE_OP;
    else if (modeChange.ch == 'p')
        modeFlag = MODE_PRIV;
    else if (modeChange.ch == 's')
        modeFlag = MODE_SECRET;
    else if (modeChange.ch == 'n')
        modeFlag = MODE_OUTSIDE;
    if (modeChange.ch == 'o') // 매개변수가 있어야 하는 플래그
    {
        IRCSession* session = server->FindByNick(modeChange.target);
        if (session == NULL)
            return ModeChange::CHANGEMODE_NOSUCHNICK;
        if (!ChangeParticipantFlag(*session, adding, modeFlag))
            return ModeChange::CHANGEMODE_NOTAFFECTED;
    }
    else
    {
        if (adding)
        {
            if (HasFlag(modeFlag))
                return ModeChange::CHANGEMODE_NOTAFFECTED;
            _flags |= modeFlag;
        }
        else
        {
            if ((_flags & modeFlag) == 0)
                return ModeChange::CHANGEMODE_NOTAFFECTED;
            _flags &= ~modeFlag;
        }
    }
    return ModeChange::CHANGEMODE_SUCCESS;
}

bool    IRCChannel::IsListShownTo(const IRCSession& session) const
{
    if((_flags & MODE_SECRET) || (_flags & MODE_PRIV))
        return IsJoined(session);
    return true;
}

bool    IRCChannel::IsJoined(const IRCSession& session) const
{
    return session.IsJoinedChannel(GetChannelName());
}

bool    IRCChannel::ChangeParticipantFlag(IRCSession& session, bool adding, ModeFlag flag)
{
    if (adding)
    {
        if (HasParticipantFlag(session, flag))
            return false;
        _participants[&session] |= flag;
    }
    else
    {
        if (!HasParticipantFlag(session, flag))
            return false;
        _participants[&session] &= ~flag;
    }
    return true;
}
