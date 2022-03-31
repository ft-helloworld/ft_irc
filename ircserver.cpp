/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 19:34:57 by yejsong           #+#    #+#             */
/*   Updated: 2022/04/01 02:41:20 by smun             ###   ########.fr       */
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
    // 메시지에 파라미터가 없었다면 에러
    if (msg.SizeParam() == 0)
        throw irc_exception(ERR_NONICKNAMEGIVEN, "No nickname given");

    const std::string& nick = msg.GetParam(0);

    // 닉네임이 유효한 문자열 조합이 아니라면 에러
    if (!IRCString::IsValidNick(nick))
        throw irc_exception(ERR_ERRONEUSNICKNAME, "Erroneous nickname");

    // 서버에 이미 등록된 닉네임이라면 에러
    if (_clients.find(nick) != _clients.end())
        throw irc_exception(ERR_NICKNAMEINUSE, "Nickname is already in use", nick);

    // 기존의 닉네임 삭제
    _clients.erase(session.GetNickname());
    Log::Vp("IRCServer::CheckNickname", "닉네임 '%s' 가 서버에서 삭제되었습니다.", session.GetNickname().c_str());

    // 새로운 닉네임 등록
    _clients[nick] = session.GetSocketId();
    Log::Vp("IRCServer::CheckNickname", "닉네임 '%s' 가 서버에 등록되었습니다.", nick.c_str());

    // 세션에도 닉네임 설정
    session.SetNickname(nick);

    // 세션에 닉네임이 등록되었다는 플래그 설정
    Log::Vp("IRCServer::CheckNickname", "현재 세션에서 FLAG_NICKNAME 플래그가 설정되었습니다.");
    session.RegisterStep(IRCSession::FLAG_NICKNAME);
}

void    IRCServer::OnUsername(IRCSession& session, IRCMessage& msg)
{
    // USER 커맨드는 꼬리표 포함 총 4개의 파라미터 필요. 부족하면 에러.
    if ((msg.SizeParam() < 3 || msg.GetTrailing().empty()) && msg.SizeParam() < 4)
        throw irc_exception(ERR_NEEDMOREPARAMS, "Not enough parameters");

    // 이미 USER, NICK 모두 등록되었다면 에러
    if (session.IsFullyRegistered())
        throw irc_exception(ERR_ALREADYREGISTRED, "You may not reregister");

    const std::string& username = msg.GetParam(0);

    // 세션에도 유저네임 설정
    session.SetUsername(username);

    // 세션에 유저네임이 등록되었다는 플래그 설정
    Log::Vp("IRCServer::CheckNickname", "현재 세션에서 FLAG_USERNAME 플래그가 설정되었습니다.");
    session.RegisterStep(IRCSession::FLAG_USERNAME);
}

void    IRCServer::OnPassword(IRCSession& session, IRCMessage& msg)
{
    // PASS 커맨드에 파라미터가 없었다면 에러
    if (msg.SizeParam() == 0)
        throw irc_exception(ERR_NEEDMOREPARAMS, "Not enough parameters", "PASS");

    // 이미 USER, NICK 모두 등록되었다면 에러
    if (session.IsFullyRegistered())
        throw irc_exception(ERR_ALREADYREGISTRED, "You may not reregister");

    // 세션에 입력받은 접속 패스워드 설정
    session.SetPassword(msg.GetParam(0));
}

void    IRCServer::OnQuit(IRCSession& session, IRCMessage& msg)
{
    // 꼬리표에서 접속 종료 사유를 클라이언트로부터 얻음.
    std::string quitReason = msg.GetTrailing();
    if (quitReason.empty())
        quitReason = "접속 종료";

    //TODO 채널에 접속중일 경우 알림

    // 해당 종료 사유를 이용해 접속 종료
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
