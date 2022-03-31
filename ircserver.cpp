/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 19:34:57 by yejsong           #+#    #+#             */
/*   Updated: 2022/04/01 02:04:26 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircserver.hpp"
#include "irc_exception.hpp"
#include "ircsession.hpp"
#include "ircstring.hpp"
#include "ircmessage.hpp"

IRCServer::IRCServer(const std::string& password)
    : _password(password) {}

IRCServer::~IRCServer() {}

int     IRCServer::FindByNick(const std::string& nick) const
{
    ClientMap::const_iterator it = _clients.find(nick);
    if (it == _clients.end())
        return 0;
    return it->second;
}

void    IRCServer::OnNickname(IRCSession& session, IRCMessage& msg)
{
    if (msg.SizeParam() == 0)
        throw irc_exception(ERR_NONICKNAMEGIVEN, "No nickname given");

    const std::string& nick = msg.GetParam(0);
    if (!IRCString::IsValidNick(nick))
        throw irc_exception(ERR_ERRONEUSNICKNAME, "Erroneous nickname");
    if (_clients.find(nick) != _clients.end())
        throw irc_exception(ERR_NICKNAMEINUSE, "Nickname is already in use", nick);

    _clients.erase(session.GetNickname());
    Log::Vp("IRCServer::CheckNickname", "닉네임 '%s' 가 서버에서 삭제되었습니다.", session.GetNickname().c_str());

    _clients[nick] = session.GetSocketId();
    Log::Vp("IRCServer::CheckNickname", "닉네임 '%s' 가 서버에 등록되었습니다.", nick.c_str());

    session.SetNickname(nick);

    Log::Vp("IRCServer::CheckNickname", "현재 세션에서 FLAG_NICKNAME 플래그가 설정되었습니다.");
    session.RegisterStep(IRCSession::FLAG_NICKNAME);
}

void    IRCServer::OnUsername(IRCSession& session, IRCMessage& msg)
{
    if (msg.SizeParam() < 4)
        throw irc_exception(ERR_NEEDMOREPARAMS, "Not enough parameters");
    if (session.IsFullyRegistered())
        throw irc_exception(ERR_ALREADYREGISTRED, "You may not reregister");

    const std::string& username = msg.GetParam(0);
    session.SetUsername(username);

    Log::Vp("IRCServer::CheckNickname", "현재 세션에서 FLAG_USERNAME 플래그가 설정되었습니다.");
    session.RegisterStep(IRCSession::FLAG_USERNAME);
}

void    IRCServer::OnPassword(IRCSession& session, IRCMessage& msg)
{
    if (msg.SizeParam() == 0)
        throw irc_exception(ERR_NEEDMOREPARAMS, "Not enough parameters", "PASS");
    if (session.IsFullyRegistered())
        throw irc_exception(ERR_ALREADYREGISTRED, "You may not reregister");
    session.SetPassword(msg.GetParam(0));
}

void    IRCServer::OnQuit(IRCSession& session, IRCMessage& msg)
{
    std::string quitReason = msg.GetTrailing();
    if (quitReason.empty())
        quitReason = "접속 종료";

    //TODO 채널에 접속중일 경우 알림
    session.Disconnect(quitReason);
}

void    IRCServer::UnregisterNickname(const std::string& nick)
{
    _clients.erase(nick);
    Log::Vp("IRCServer::CheckNickname", "닉네임 '%s' 가 서버에서 삭제되었습니다.", nick.c_str());
}

bool IRCServer::IsPasswordMatched(const std::string& password) const
{
    return _password == password;
}
