/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircchannel.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 12:37:36 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 22:27:00 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCCHANNEL_HPP
#define IRCCHANNEL_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include "modelist.hpp"

class IRCSession;
class IRCMessage;
class IRCServer;

class IRCChannel
{
public:
    typedef int ModeFlag;
    typedef std::map<IRCSession*, ModeFlag> ParticipantMap;

    enum { MODE_OP = 1 << 0, MODE_PRIV = 1 << 1, MODE_SECRET = 1 << 2, MODE_OUTSIDE = 1 << 5 };

private:
    const std::string               _name;
    const time_t                    _created;
    std::string                     _topic;
    time_t                          _set_topic;
    std::string                     _mask_topic;
    /**
     * @brief 채널의 참가자들을 저장하는 std::map 객체. Key: IRC세션 포인터 / Value: 채널 내 유저 플래그
     *
     */
    ParticipantMap  _participants;
    int             _flags;

    IRCChannel();
    IRCChannel(const IRCChannel&);
    IRCChannel& operator= (const IRCChannel&);

public:
    IRCChannel(const std::string& name);
    ~IRCChannel();

    /**
     * @brief 현재 채널의 참가자들을 세션에 전송합니다.
     *
     * @param session 참가자들의 닉네임을 전송할 세션.
     */
    void    SendNames(IRCSession& session, bool sendEndReply = true) const;

    /**
     * @brief 현재 채널에 세션을 참가시킵니다.
     *
     * @param session 채널에 참가시킬 세션.
     */
    void    Join(IRCSession& session);

    /**
     * @brief 현재 채널에서 세션을 퇴장시킵니다.
     *
     * @param session 퇴장시킬 세션
     * @param cmd 퇴장에 사용한 명령
     */
    void    Part(IRCSession& session, const std::string& cmd);

    /**
     * @brief 현재 채널에 참여중인 참가자들에게 메시지를 모두 전송합니다.
     *
     * @param msg 전송할 메시지
     * @param except 채널의 참가자들 중 except에 해당하는 세션은 제외하고 전송합니다.
     * 일반적으로 자기 자신을 제외한 나머지 세션들에게 메시지를 보내기 위해 사용합니다.
     */
    void    Send(const IRCMessage& msg, IRCSession* except = NULL) const;

    /**
     * @brief 참가자들의 세션의 포인터를 매개변수로 주어진 std::set에 insert 합니다.
     *
     * @param targets 참가자 세션의 포인터들이 저장될 std::set
     * @param except insert 생략할 세션 포인터. 일반적으로 자기 자신을 제외하고 나머지 세션들을 찾아 추가하기 위해 사용.
     */
    void    GatherParticipants(std::set<IRCSession*>& targets, IRCSession* except = NULL);
    void    SetChannelTopic(const std::string& topic, const time_t time, const std::string& mask);
    void    MakeChannelModeString(std::string& ret, bool val);
    std::string&    RetrunChannelModeString(IRCSession& session, std::string& tmp, std::string& res);
    int     SetChannelMode(IRCServer* server, const ModeChange& modeChange);
    bool    IsListShownTo(const IRCSession& session) const;
    bool    IsJoined(const IRCSession& session) const;
    bool    ChangeParticipantFlag(IRCSession& session, bool adding, ModeFlag flag);

    void    SendMode(IRCSession& session);

    inline bool HasFlag(int flags) const { return (_flags & flags) == flags; }

    inline const std::string& GetChannelName() const { return _name; }
    inline const std::string& GetChannelTopic() const { return _topic; }
    inline const time_t& GetSetTopicTime() const { return _set_topic; }
    inline const std::string& GetSetTopicMask() const { return _mask_topic; }
    inline const time_t& GetCreatedTime() const { return _created; }
    inline ParticipantMap::size_type GetParticipantNum() const { return _participants.size(); }
    inline int GetChannelFlag() const { return _flags; }
    inline bool HasParticipantFlag(IRCSession& session, ModeFlag flag) { return (_participants[&session] & flag) == flag; }
    inline bool IsEmpty() const { return _participants.size() == 0; }
};

#endif
