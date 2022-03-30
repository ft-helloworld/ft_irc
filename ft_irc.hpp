/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 17:31:23 by smun              #+#    #+#             */
/*   Updated: 2022/03/30 22:16:42 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_HPP
#define FT_IRC_HPP

#include "std.hpp"
#include "log.hpp"
#include "channel.hpp"
#include "string.hpp"
#include "session.hpp"
#include "sessionfactory.hpp"
#include "numerics.hpp"
#include <string>
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <map>

#define HOSTNAME "ft-helloworld"

class IRCServer;

/**
 * @brief IRC의 로직을 수행할 세션 클래스입니다.
 *
 */
class IRCSession : public Session
{
private:
    std::string   _nickname;
    std::string   _username;
    IRCServer*  _server;

    IRCSession();
    IRCSession(const IRCSession&);
    IRCSession& operator=(const IRCSession&);
public:
    IRCSession(IRCServer* server, Channel* channel, int socketfd, int socketId, const std::string& addr);
    virtual ~IRCSession();

    virtual void Process(const std::string& line);

    void    Reply(int statuscode, const std::string& line);

    void                SetNickname(const std::string& nickname);
    const std::string&  GetNickname() const;
    void                SetUsername(const std::string& username);
    const std::string&  GetUsername() const;
};

class IRCServer
{
private:
    std::map<const std::string, int> _clients;

    IRCServer(const IRCServer&);
    IRCServer& operator= (const IRCServer&);

public:
    IRCServer();
    ~IRCServer();

    void    CheckNickname(IRCSession& session, const std::string& beforeNick, const std::string& afterNick);
};

/**
 * @brief IRC세션을 채널에서 생성시켜줄 팩토리 클래스입니다.
 *
 */
class IRCSessionFactory : public ISessionFactory
{
private:
    IRCServer*  _server;

    IRCSessionFactory();
    IRCSessionFactory(const IRCSessionFactory&);
    IRCSessionFactory& operator=(const IRCSessionFactory&);
public:
    IRCSessionFactory(IRCServer* server);
    ~IRCSessionFactory();

    virtual Session*    CreateSession(Channel* channel, int socketfd, int socketId, const std::string& addr);
};

class irc_exception : public std::exception
{
private:
    const std::string _message;
    irc_exception();
    irc_exception& operator= (const irc_exception&);

public:
    irc_exception(const irc_exception&);
    virtual ~irc_exception() throw();
    irc_exception(int errcode, const std::string& message) throw();

    const char* what() const throw();
};

//TODO
// IRC서버, IRC채널(방), 개인메시지, 관리자 등등

#endif
