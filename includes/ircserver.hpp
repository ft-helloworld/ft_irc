/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seungyel <seungyel@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 00:40:50 by smun              #+#    #+#             */
/*   Updated: 2022/04/05 19:58:47 by seungyel         ###   ########.fr       */
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

class IRCSession;
class IRCMessage;

class IRCServer
{
public:
    typedef std::map<const std::string, IRCSession*, IRCComparer> ClientMap;
    typedef std::map<const std::string, SharedPtr<IRCChannel>, IRCComparer> ChannelMap;

private:

    const std::string _password;

    ClientMap   _clients;
    ChannelMap  _channels;

    IRCServer(const IRCServer&);
    IRCServer& operator= (const IRCServer&);

    void    CheckChannelExpire(IRCChannel* channel);

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
	void    OnKill(IRCSession& session, IRCMessage& msg);
    void    UnregisterNickname(const std::string& nick);
    void    LeaveChannel(IRCSession& session, const std::string& chanName, const std::string& cmd);
    bool    IsPasswordMatched(const std::string& password) const;

    IRCSession* FindByNick(const std::string& nick) const;

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
