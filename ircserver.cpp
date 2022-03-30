/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 19:34:57 by yejsong           #+#    #+#             */
/*   Updated: 2022/03/31 00:47:38 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircserver.hpp"
#include "irc_exception.hpp"
#include "ircsession.hpp"

IRCServer::IRCServer() {}
IRCServer::~IRCServer() {}

void    IRCServer::CheckNickname(IRCSession& session, const std::string& beforeNick, const std::string& afterNick)
{
    if (afterNick.empty())
        throw irc_exception(ERR_NONICKNAMEGIVEN, ":No nickname given");
    if (afterNick.length() > 9)
        throw irc_exception(ERR_ERRONEUSNICKNAME, ":Erroneous nickname");
    for (size_t i = 0; i < afterNick.length(); ++i)
    {
        if (i == 0)
        {
            // :bassoon.irc.ozinger.org 432 625AAEZ25 0aaaaaa :Erroneous Nickname
            if (!String::IsLetter(afterNick[i]) && !String::IsSpecial(afterNick[i]))
                throw irc_exception(ERR_ERRONEUSNICKNAME, afterNick + " :Erroneous nickname");
        }
        else
        {
            if (!String::IsDigit(afterNick[i]) && afterNick[i] != '-' && !String::IsLetter(afterNick[i]) && !String::IsSpecial(afterNick[i]))
                throw irc_exception(ERR_ERRONEUSNICKNAME, afterNick + " :Erroneous nickname");
        }
    }
    if (_clients.find(afterNick) != _clients.end())
        throw irc_exception(ERR_NICKNAMEINUSE, afterNick + " :Nickname is already in use");
    _clients.erase(beforeNick);
    _clients[afterNick] = session.GetSocketId();
    session.SetNickname(afterNick);
}
