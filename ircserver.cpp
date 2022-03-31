/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 19:34:57 by yejsong           #+#    #+#             */
/*   Updated: 2022/03/31 18:38:01 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircserver.hpp"
#include "irc_exception.hpp"
#include "ircsession.hpp"
#include "ircstring.hpp"

IRCServer::IRCServer() {}
IRCServer::~IRCServer() {}

void    IRCServer::OnNickname(IRCSession& session, const std::string& beforeNick, const std::string& afterNick)
{
    if (afterNick.empty())
        throw irc_exception(ERR_NONICKNAMEGIVEN, "No nickname given");
    if (!IRCString::IsValidNick(afterNick))
        throw irc_exception(ERR_ERRONEUSNICKNAME, "Erroneous nickname");
    if (_clients.find(afterNick) != _clients.end())
        throw irc_exception(ERR_NICKNAMEINUSE, "Nickname is already in use", afterNick);

    _clients.erase(beforeNick);
    Log::Vp("IRCServer::CheckNickname", "닉네임 '%s' 가 서버에서 삭제되었습니다.", beforeNick.c_str());

    _clients[afterNick] = session.GetSocketId();
    Log::Vp("IRCServer::CheckNickname", "닉네임 '%s' 가 서버에 등록되었습니다.", afterNick.c_str());

    session.SetNickname(afterNick);
}

void    IRCServer::OnUsername(IRCSession& session)
{
    (void)session;
// if (args.size() < 5)
//                 throw irc_exception(ERR_NEEDMOREPARAMS, cmd + " :Not enough parameters");
//             const std::string& username = args[1];
//             if (!GetUsername().empty())
//                 throw irc_exception(ERR_ALREADYREGISTRED, ":Unauthorized command (already registered)");
//             SetUsername(username);
//             //Send(IRCMessage(RPL_WELCOME, "Welcome to the Internet Relay Network " + GetPrefix()));
//             //Reply(RPL_WELCOME, username + " :Welcome to the Internet Relay Network " + GetPrefix());
}

void    IRCServer::UnregisterNickname(const std::string& nick)
{
    _clients.erase(nick);
    Log::Vp("IRCServer::CheckNickname", "닉네임 '%s' 가 서버에서 삭제되었습니다.", nick.c_str());
}
