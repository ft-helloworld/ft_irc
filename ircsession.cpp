/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsession.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 15:03:56 by smun              #+#    #+#             */
/*   Updated: 2022/04/01 02:08:09 by smun             ###   ########.fr       */
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
#include <sstream>

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
        else if (cmd == "PASS")
            _server->OnPassword(*this, msg);
        else if (cmd == "QUIT")
            _server->OnQuit(*this, msg);
        else // :bassoon.irc.ozinger.org 421 smun WRONGCMD :Unknown command
            throw irc_exception(ERR_UNKNOWNCOMMAND, "Unknown command", cmd);
    }
    catch (const irc_exception& rex)
    {
        SendMessage(rex.message());
    }
    catch (const std::exception& ex)
    {
        //std::exception 발생 시 오류 전송 후 세션 종료
        Disconnect(ex.what());
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
    if (!IsFullyRegistered())
        return "unknown@" + GetRemoteAddress();
    return GetNickname() + "!" + GetUsername() + "@" + GetRemoteAddress();
}

void    IRCSession::Disconnect(const std::string& reason)
{
    SendMessage(IRCMessage("ERROR", "Closing link: ("+GetPrefix()+") ["+reason+"]"));
    Close();
}

void    IRCSession::SendMOTD()
{
    SendMessage(IRCMessage(HOSTNAME, RPL_MOTDSTART, "- "HOSTNAME" Message of day - "));
    SendMessage(IRCMessage(HOSTNAME, RPL_MOTD, "[Hello, World!] IRC 서버에 오신 것을 환영합니다."));
    SendMessage(IRCMessage(HOSTNAME, RPL_MOTD, "서버 호스트 이름은 "HOSTNAME" 입니다."));
    SendMessage(IRCMessage(HOSTNAME, RPL_MOTD, "현재 서버는 [smun, seungyel, yejsong]의 ft_irc 에서 실행되고 있습니다."));
    SendMessage(IRCMessage(HOSTNAME, RPL_ENDOFMOTD, "End of /MOTD command"));
}

void    IRCSession::RegisterStep(int flag)
{
    if (HasRegisterFlag(flag))
        return;
    _registerFlag |= flag;
    if (IsFullyRegistered())
    {
        Log::Vp("IRCSession::RegisterStep", "USER, NICK 명령을 모두 받았습니다. 패스워드 체크 루틴을 실행합니다.");

        // 서버 실행 인자로 주어진 패스워드와 일치하는지 검사
        if (!_server->IsPasswordMatched(GetPassword()))
            throw std::runtime_error("Password mismatched.");

        Log::Vp("IRCSession::RegisterStep", "패스워드가 일치합니다. 세션이 인증되었습니다. 환영 메시지를 전송합니다.");
        SendMOTD();
    }
}

bool    IRCSession::HasRegisterFlag(int flag) const { return (_registerFlag & flag) == flag; }
bool    IRCSession::IsFullyRegistered() const
{
    return HasRegisterFlag(FLAG_NICKNAME) && HasRegisterFlag(FLAG_USERNAME);
}

void    IRCSession::SendMessage(const IRCMessage& msg)
{
    std::ostringstream oss;

    // 맨 처음, :prefix 형태로 추가
    if (!msg.GetPrefix().empty())
        oss << ":" << msg.GetPrefix() << " ";

    // 다음, cmd 추가
    oss << msg.GetCommand();

    // 숫자 응답일 경우, 등록되었다면 맨 앞 파라미터는 닉네임이 됨, 등록되지 않았다면 맨 앞 파라미터는 *이 됨.
    if (msg.IsNumeric())
    {
        if (IsFullyRegistered())
            oss << " " << GetNickname();
        else
            oss << " " << "*";
    }

    // 다음, 각 param 추가.
    IRCMessage::ParamVectorConstIterator it;
    for (it = msg.BeginParam(); it != msg.EndParam(); ++it)
        oss << " " << *it;

    // 맨 마지막, :trailing 형태로 추가
    if (!msg.GetTrailing().empty())
        oss << " :" << msg.GetTrailing();

    // 만들어진 최종 메시지를 전송.
    Send(oss.str());
}
