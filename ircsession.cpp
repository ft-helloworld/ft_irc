/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsession.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 15:03:56 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 21:56:58 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircserver.hpp"
#include "ircsession.hpp"
#include "irc_exception.hpp"
#include "ircmessage.hpp"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <map>

IRCSession::IRCSession(IRCServer* server, Channel* channel, int socketfd, int socketId, const std::string& addr)
    : Session(channel, socketfd, socketId, addr)
    , _nickname()
    , _username()
    , _server(server)
    , _registerFlag(0)
    , _password()
    {}

IRCSession::~IRCSession()
{
    _server->UnregisterNickname(_nickname);
}

void IRCSession::Process(const std::string& line)
{
    // 부모 클래스의 Process 먼저 실행
    Session::Process(line);

    // 날아온 한 줄 처리

    // 명령어 처리
    try
    {
        IRCMessage msg = IRCMessage::Parse(line);
        if (msg.IsEmpty())
            return;
        const std::string& cmd = msg.GetCommand();
        if (cmd == "NICK")
            _server->OnNickname(*this, msg);
        else if (cmd == "USER")
            _server->OnUsername(*this, msg);
        else // :bassoon.irc.ozinger.org 421 smun WRONGCMD :Unknown command
            throw irc_exception(ERR_UNKNOWNCOMMAND, "Unknown command", GetNickname(), cmd);
    }
    catch (const std::exception& ex)
    {
        Send(ex.what());
    }
}

void    IRCSession::SetNickname(const std::string& nickname) { _nickname = nickname; }
void    IRCSession::SetUsername(const std::string& username) { _username = username; }
void    IRCSession::SetPassword(const std::string& password) { _password = password; }

const std::string&  IRCSession::GetNickname() const { return _nickname; }
const std::string&  IRCSession::GetUsername() const { return _username; }
const std::string&  IRCSession::GetPassword() const { return _password; }

const std::string   IRCSession::GetPrefix() const
{
    return GetNickname() + "!" + GetUsername() + "@" + GetRemoteAddress();
}

void    IRCSession::SendMessage(const IRCMessage& msg)
{
    Send(msg.GetMessage());
}

void    IRCSession::SendWelcome()
{
    IRCMessage msg(RPL_WELCOME, "Welcome to the Internet Relay Network " + GetPrefix());
    msg.AddParam(GetUsername());
    SendMessage(msg);
}

void    IRCSession::RegisterStep(int flag)
{
    if (HasRegisterFlag(flag))
        return;
    _registerFlag |= flag;
    if (IsFullyRegistered())
    {
        Log::Vp("IRCSession::RegisterStep", "완전히 등록되어 패스워드 체크 루틴을 실행합니다.");
        //if (GetPassword() != _server->GetPassword())
        //    throw 0;

        Log::Vp("IRCSession::RegisterStep", "패스워드가 일치합니다. 세션이 인증되었습니다. 환영 메시지를 전송합니다.");
        SendWelcome();
    }
}

bool    IRCSession::HasRegisterFlag(int flag) const { return (_registerFlag & flag) == flag; }
bool    IRCSession::IsFullyRegistered() const
{
    return HasRegisterFlag(FLAG_NICKNAME) && HasRegisterFlag(FLAG_USERNAME);
}
