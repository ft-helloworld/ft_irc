/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 00:40:50 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 22:22:02 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <iosfwd>
#include <map>
#include <set>
#include "string.hpp"
#include "ircchannel.hpp"
#include "irccomparer.hpp"
#include "shared_ptr.hpp"
#include "timerhandler.hpp"
#include "modelist.hpp"

class IRCSession;
class IRCMessage;
class IRCBot;

class IRCServer : public ITimerHandler
{
public:
    typedef std::map<const std::string, IRCSession*, IRCComparer> ClientMap;
    typedef std::map<const std::string, SharedPtr<IRCChannel>, IRCComparer> ChannelMap;

private:

    struct ModeTarget
    {
        bool isChannel;
        IRCChannel* channel;
        IRCSession* session;

        inline ModeTarget(bool isChannel)
            : isChannel(isChannel) {}
    };

    const std::string _password;

    ClientMap   _clients;
    ChannelMap  _channels;

    IRCServer(const IRCServer&);
    IRCServer& operator= (const IRCServer&);

    void    CheckChannelExpire(IRCChannel* channel);
    void    ProcessModeChange(IRCSession& session, ModeTarget target, ModeList& modeList);

public:
    IRCServer(const std::string& password);
    ~IRCServer();

    void    OnNickname(IRCSession& session, IRCMessage& msg);
    void    OnUsername(IRCSession& session, IRCMessage& msg);
    void    OnPassword(IRCSession& session, IRCMessage& msg);
    void    OnQuit(IRCSession& session, IRCMessage& msg);
    void    OnJoin(IRCSession& session, IRCMessage& msg);
    void    OnPart(IRCSession& session, IRCMessage& msg);
    void    OnNames(IRCSession& session, IRCMessage& msg);
    void    OnPrivMsg(IRCSession& session, IRCMessage& msg, const std::string& cmd);
    void    OnTopic(IRCSession& session, IRCMessage& msg);
    void    OnList(IRCSession& session, IRCMessage& msg);
    void    OnMode(IRCSession& session, IRCMessage& msg);
	void    OnKill(IRCSession& session, IRCMessage& msg);
    void    OnMOTD(IRCSession& session);
    void    UnregisterNickname(const std::string& nick);
    void    JoinChannel(IRCSession& session, const std::string& chanName);
    void    LeaveChannel(IRCSession& session, const std::string& chanName, const std::string& cmd);
    bool    IsPasswordMatched(const std::string& password) const;
    void    OnTimer();
    size_t  GetInterval() const;

    IRCSession* FindByNick(const std::string& nick) const;
    IRCChannel* FindChannel(const std::string& channel);

    void    RegisterBot(IRCBot& bot);

    template<typename ChannelNameIterator>
    void    GatherNeighbors(std::set<IRCSession*>& neighbors, ChannelNameIterator first, ChannelNameIterator last, IRCSession* except = NULL)
    {
        for (; first != last; ++first)
        {
            ChannelMap::iterator chanIt = _channels.find(*first);
            if (chanIt == _channels.end())
                continue;
            IRCChannel* channel = chanIt->second.Load();
            channel->GatherParticipants(neighbors, except);
        }
    }

};

#endif
