/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/06 15:35:55 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 18:36:09 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCBOT_HPP
#define IRCBOT_HPP

#include <string>
#include <vector>
#include "ircsession.hpp"

class IRCMessage;

class IRCBot : public IRCSession
{
private:
    typedef std::vector<std::string> ArgsVector;

    struct bot_exception
    {
        const std::string message;

        inline bot_exception(const std::string& msg)
            : message(msg) {}
    };

    IRCBot();
    IRCBot(const IRCBot&);
    IRCBot& operator=(const IRCBot&);

    void    OnHelp(const std::string& fromNick, ArgsVector& args);
    void    OnJoin(const std::string& fromNick, ArgsVector& args);
    void    OnMsg(const std::string& fromNick, ArgsVector& args);
    void    OnRockScissorPaper(const std::string& fromNick, const std::string& channel, ArgsVector& args);
    void    OnDice(const std::string& fromNick, const std::string& channel);

protected:
    virtual void    OnMessage(const std::string& fromNick, const std::string& channel, const std::string& commandline);
    virtual void    Send(const void* buf, size_t len);

    size_t    SendTo(const std::string& param, bool notice, const std::string& msg);
    void      SendToChannelOrUser(const std::string& fromNick, const std::string& channel, const std::string& msg);

public:
    IRCBot(IRCServer* server, const std::string& nickname, const std::string& username);
    virtual ~IRCBot();

    virtual void    Close();
    virtual void    CheckActive();
};

#endif
