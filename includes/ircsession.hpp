/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsession.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yejsong <yejsong@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 00:44:02 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 14:40:32 by yejsong          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSESSION_HPP
#define IRCSESSION_HPP

#include "irccomparer.hpp"
#include <string>
#include <set>
#include <ctime>

class IRCServer;
class IRCMessage;

/**
 * @brief IRC의 로직을 수행할 세션 클래스입니다.
 *
 */
class IRCSession : public Session
{
private:
    enum { PingState_Active, PingState_Idle };

    std::string _nickname;
    std::string _username;
	int			_operflag;
    std::string _password;
    std::string _closeReason;

    std::set<const std::string, IRCComparer> _channels;

    int         _pingState;
    std::time_t _lastPingTime;
    std::string _lastPingWord;

    IRCSession();
    IRCSession(const IRCSession&);
    IRCSession& operator=(const IRCSession&);

    void    OnPing(const IRCMessage& msg);
    void    OnPong(const IRCMessage& msg);
    void    UpdateActive();

protected:
    IRCServer*  _server;
    int         _registerFlag;

public:
    enum { FLAG_NICKNAME = 1 << 0, FLAG_USERNAME = 1 << 1 };

    IRCSession(IRCServer* server, Channel* channel, int socketfd, int socketId, const std::string& addr);
    virtual ~IRCSession();

    void    Process(const std::string& line);
    void    SendMessage(const IRCMessage& msg);

    void    MessageToNeighbor(const IRCMessage& msg, IRCSession* except);
    void    SendMOTD();

    void                SetNickname(const std::string& nickname);
    const std::string&  GetNickname() const;
    void                SetUsername(const std::string& username);
    const std::string&  GetUsername() const;
    void                SetPassword(const std::string& password);
    const std::string&  GetPassword() const;
	void                SetOperFlag(int operflag);
    int  				GetOperFlag() const;
    const std::string   GetMask() const;
    const std::string   GetEmail() const;
    const std::string&  GetCloseReason() const;

    void    RegisterStep(int flag);
    bool    HasRegisterFlag(int flag) const;
	int		HasOperatorFlag(std::string str);
    bool    IsFullyRegistered() const;

    virtual void    Close();
    void            Close(const std::string& reason);

    bool    AddChannel(const std::string& name);
    bool    RemoveChannel(const std::string& name);
    bool    IsJoinedChannel(const std::string& name);
    size_t  GetJoinedChannelNum() const;

    virtual void    CheckActive();
};

#endif
