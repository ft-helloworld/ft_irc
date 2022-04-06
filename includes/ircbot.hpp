/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/06 15:35:55 by smun              #+#    #+#             */
/*   Updated: 2022/04/06 16:14:46 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCBOT_HPP
#define IRCBOT_HPP

#include <iosfwd>
#include "ircsession.hpp"

class IRCMessage;

class IRCBot : public IRCSession
{
private:
    IRCBot();
    IRCBot(const IRCBot&);
    IRCBot& operator=(const IRCBot&);

protected:
    virtual void    OnMessage(const std::string& fromNick, IRCMessage& msg) = 0;
    size_t    SendTo(const std::string& param, bool notice, const std::string& msg);

public:
    IRCBot(IRCServer* server, const std::string& nickname, const std::string& username);
    virtual ~IRCBot();

    virtual void    Close();
    virtual void    CheckActive();
    virtual void    Send(const void* buf, size_t len);
};

#endif
