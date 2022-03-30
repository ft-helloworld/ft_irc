/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yejsong <yejsong@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 19:34:57 by yejsong           #+#    #+#             */
/*   Updated: 2022/03/30 20:36:27 by yejsong          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

IRCServer::IRCServer() {}
IRCServer::~IRCServer() {}

void    IRCServer::CheckNickname(IRCSession& session, const std::string& beforeNick, const std::string& afterNick)
{
    if (afterNick.empty())
        throw irc_exception("431", ":No nickname given");
    if (afterNick.length() > 9)
        throw irc_exception("432", ":Erroneous nickname");
    for (size_t i = 0; i < afterNick.length(); ++i)
    {
        if (i != 0 && !String::IsDigit(afterNick[i]) && afterNick[i] != '-')
            throw irc_exception("432", ":Erroneous nickname");
        if (!String::IsLetter(afterNick[i]) && !String::IsSpecial(afterNick[i]))
            throw irc_exception("432", ":Erroneous nickname");
    }
    if (_clients.find(afterNick) != _clients.end())
        throw irc_exception("433", afterNick + " :Nickname is already in use");
    _clients.erase(beforeNick);
    _clients[afterNick] = session.GetSocketId();
    session.SetNickname(afterNick);
}
