/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsession.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 00:44:02 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 17:13:26 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSESSION_HPP
#define IRCSESSION_HPP

#include <string>

class IRCServer;
class IRCMessage;

/**
 * @brief IRC의 로직을 수행할 세션 클래스입니다.
 *
 */
class IRCSession : public Session
{
private:
    std::string _nickname;
    std::string _username;
    IRCServer*  _server;

    IRCSession();
    IRCSession(const IRCSession&);
    IRCSession& operator=(const IRCSession&);
public:
    IRCSession(IRCServer* server, Channel* channel, int socketfd, int socketId, const std::string& addr);
    virtual ~IRCSession();

    virtual void Process(const std::string& line);

    void    SendMessage(const IRCMessage& msg);

    void                SetNickname(const std::string& nickname);
    const std::string&  GetNickname() const;
    void                SetUsername(const std::string& username);
    const std::string&  GetUsername() const;

    const std::string   GetPrefix() const;
};


#endif
