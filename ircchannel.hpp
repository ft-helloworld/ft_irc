/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircchannel.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 12:37:36 by smun              #+#    #+#             */
/*   Updated: 2022/04/02 02:50:24 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCCHANNEL_HPP
#define IRCCHANNEL_HPP

#include <string>
#include <vector>
#include <map>

class IRCSession;
class IRCMessage;

class IRCChannel
{
public:
    typedef int ModeFlag;
    typedef std::map<IRCSession*, ModeFlag> ParticipantMap;

    enum { MODE_OP = 1 << 0, MODE_PRIV = 1 << 1, MODE_SECRET = 1 << 2 };

private:
    const std::string               _name;

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
     */
    void    Send(const IRCMessage& msg, IRCSession* except = NULL) const;

    // 여기는 안해도 될 지도..
    void    Mode(IRCSession& session, bool add, int flags, const std::string& target = "");
    void    Kick(IRCSession& session, const std::string& target);

    inline bool HasFlag(int flags) const { return (_flags & flags) == flags; }
    inline const std::string& GetChannelName() const { return _name; }
    inline bool IsEmpty() const { return _participants.size() == 0; }
};

#endif
