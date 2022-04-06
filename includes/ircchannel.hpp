/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircchannel.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yejsong <yejsong@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 12:37:36 by smun              #+#    #+#             */
/*   Updated: 2022/04/06 19:47:27 by yejsong          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCCHANNEL_HPP
#define IRCCHANNEL_HPP

#include <string>
#include <vector>
#include <map>
#include <set>

class IRCSession;
class IRCMessage;

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
    int             _flags; //flag 말고 mode가 더 적절하지 않을지??

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
    void    MakeChannelModeString(std::string& ret);
    std::string&    RetrunChannelModeString(IRCSession& session, std::string& tmp, std::string& res);
    void    SetChannelMode(char neg, std::string& res);

    // 여기는 안해도 될 지도..
    void    SendTopic(IRCSession& session);
    void    Mode(IRCSession& session, bool add, int flags, const std::string& target = "");
    void    Kick(IRCSession& session, const std::string& target);

    inline bool HasFlag(int flags) const { return (_flags & flags) == flags; }
    inline const std::string& GetChannelName() const { return _name; }
    inline const std::string& GetChannelTopic() const { return _topic; }
    inline const time_t& GetSetTopicTime() const { return _set_topic; }
    inline const std::string& GetSetTopicMask() const { return _mask_topic; }
    inline const time_t& GetCreatedTime() const { return _created; }
    inline size_t GetParticipantsNum() { return _participants.size(); }
    inline int GetParticipantFlag(IRCSession& session) {return _participants[&session]; }
    inline bool IsEmpty() const { return _participants.size() == 0; }
};

#endif
