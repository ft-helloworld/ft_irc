/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 17:31:23 by smun              #+#    #+#             */
/*   Updated: 2022/03/30 15:10:35 by smun             ###   ########.fr       */
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
#include <string>
#include <stdexcept>

/**
 * @brief IRC의 로직을 수행할 세션 클래스입니다.
 *
 */
class IRCSession : public Session
{
private:
    IRCSession();
    IRCSession(const IRCSession&);
    IRCSession& operator=(const IRCSession&);
public:
    IRCSession(Channel* channel, int socketfd, int socketId, const std::string& addr);
    virtual ~IRCSession();

    virtual void Process(const std::string& line);
};

/**
 * @brief IRC세션을 채널에서 생성시켜줄 팩토리 클래스입니다.
 *
 */
class IRCSessionFactory : public ISessionFactory
{
public:
    IRCSessionFactory();
    ~IRCSessionFactory();
    IRCSessionFactory(const IRCSessionFactory&);
    IRCSessionFactory& operator=(const IRCSessionFactory&);

    virtual Session*    CreateSession(Channel* channel, int socketfd, int socketId, const std::string& addr);
};

//TODO
// IRC서버, IRC채널(방), 개인메시지, 관리자 등등

#endif
