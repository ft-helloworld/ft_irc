/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsessionfactory.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 00:45:35 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 00:46:25 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSESSIONFACTORY_HPP
#define IRCSESSIONFACTORY_HPP

#include "sessionfactory.hpp"

class IRCServer;

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


#endif
