/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsession.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 15:03:56 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 21:44:02 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircserver.hpp"
#include "ircsession.hpp"
#include "irc_exception.hpp"
#include "ircnumericmessage.hpp"
#include "ircmessage.hpp"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <map>
#include <sstream>
#include <vector>
#include <set>
#include <ctime>

IRCSession::IRCSession(IRCServer* server, Channel* channel, int socketfd, int socketId, const std::string& addr)
    : Session(channel, socketfd, socketId, addr)
    , _nickname()
    , _username()
	, _flag(0)
    , _password()
    , _closeReason()
    , _channels()
    , _pingState(PingState_Active)
    , _lastPingTime(std::time(NULL))
    , _lastPingWord()
    , _server(server)
    , _registerFlag(0)
    {}

IRCSession::~IRCSession()
{
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
        else if (cmd == "PING")
            OnPing(msg);
        else if (cmd == "PONG")
            OnPong(msg);
        else
        {
            if (!IsFullyRegistered())
                throw irc_exception(ERR_NOTREGISTERED, cmd, "You are not registered");

            // 로그인 완료 한 사용자만 사용할 수 있는 커맨드들
            if (cmd == "JOIN")
                _server->OnJoin(*this, msg);
            else if (cmd == "PART")
                _server->OnPart(*this, msg);
            else if (cmd == "NAMES")
                _server->OnNames(*this, msg);
            else if (cmd == "PRIVMSG" || cmd == "NOTICE")
                _server->OnPrivMsg(*this, msg, cmd);
            else if (cmd == "TOPIC")
                _server->OnTopic(*this, msg);
            else if (cmd == "LIST")
                _server->OnList(*this, msg);
            else if (cmd == "MODE")
                _server->OnMode(*this, msg);
			else if (cmd == "KILL")
            	_server->OnKill(*this, msg);
			else if (cmd == "MODE")
            	_server->OnMode(*this, msg);
            else if (cmd == "MOTD")
                _server->OnMOTD(*this);
            else
                throw irc_exception(ERR_UNKNOWNCOMMAND, cmd, "Unknown command");
        }
    }
    catch (const irc_exception& rex)
    {
        SendMessage(rex.message());
    }
    catch (const std::exception& ex)
    {
        //std::exception 발생 시 오류 전송 후 세션 종료
        Close(ex.what());
    }
}

void	IRCSession::SetNickname(const std::string& nickname) { _nickname = nickname; }
void	IRCSession::SetUsername(const std::string& username) { _username = username; }
void	IRCSession::SetPassword(const std::string& password) { _password = password; }

int     IRCSession::SetFlag(const ModeChange& modeChange)
{
    const bool adding = modeChange.sign == '+';

    int modeFlag = 0;
    if (modeChange.ch == 'o')
        modeFlag = FLAG_OP;
    if (adding)
    {
        if (HasFlag(modeFlag))
            return ModeChange::CHANGEMODE_NOTAFFECTED;
        _flag |= modeFlag;
    }
    else
    {
        if (!HasFlag(modeFlag))
            return ModeChange::CHANGEMODE_NOTAFFECTED;
        _flag &= ~modeFlag;
    }
    return ModeChange::CHANGEMODE_SUCCESS;
}

const std::string&  IRCSession::GetNickname() const { return _nickname; }
const std::string&  IRCSession::GetUsername() const { return _username; }
const std::string&  IRCSession::GetPassword() const { return _password; }

const std::string&  IRCSession::GetCloseReason() const { return _closeReason; }
size_t              IRCSession::GetJoinedChannelNum() const { return _channels.size(); }

bool    IRCSession::AddChannel(const std::string& name)
{
    return _channels.insert(name).second;
}

bool    IRCSession::RemoveChannel(const std::string& name)
{
    return _channels.erase(name) > 0;
}

bool    IRCSession::IsJoinedChannel(const std::string& name) const
{
    return _channels.find(name) != _channels.end();
}

const std::string   IRCSession::GetMask() const
{
    if (!IsFullyRegistered())
        return "unknown@" + GetRemoteAddress();
    return GetNickname() + "!" + GetEmail();
}

const std::string   IRCSession::GetEmail() const
{
    if (!IsFullyRegistered())
        return "unknown@" + GetRemoteAddress();
    return GetUsername() + "@" + GetRemoteAddress();
}

void    IRCSession::Close()
{
    // 지정된 종료 사유가 없다면, 기본적인 종료 사유를 강제로 지정.
    if (_closeReason.empty())
        _closeReason = "Closing link: ("+GetEmail()+") [Network Error]";

    // 자기 자신에게 종료 메시지 먼저 전송
    SendMessage(IRCMessage("", "ERROR", _closeReason));

    // 서버에서 닉네임 등록 해제
    _server->UnregisterNickname(_nickname);

    // 참여중인 채널에서 모두 퇴장

    MessageToNeighbor(IRCMessage(GetMask(), "QUIT", _closeReason), this);
    std::vector<const std::string> channels(_channels.begin(), _channels.end());
    std::vector<const std::string>::iterator it;
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        try
        {
            _server->LeaveChannel(*this, *it, "QUIT");
        }
        catch (const irc_exception&) {}
    }

    // 실제 세션 종료 처리
    Session::Close();
}

void    IRCSession::Close(const std::string& reason)
{
    // 나중에 입장된 채널들에 퇴장 알림 전송을 위해 종료 사유 저장
    _closeReason = "Closing link: ("+GetEmail()+") ["+reason+"]";

    Close();
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
            throw std::runtime_error("Pas sword mismatched.");

        Log::Vp("IRCSession::RegisterStep", "패스워드가 일치합니다. 세션이 인증되었습니다. 환영 메시지를 전송합니다.");
        SendMessage(IRCNumericMessage(RPL_WELCOME, "Welcome to the [Hello, World!] IRC Network " + GetMask()));
        SendMessage(IRCNumericMessage(RPL_YOURHOST, "Your host is "HOSTNAME", running version ft_irc"));
        SendMessage(IRCNumericMessage(RPL_CREATED, "This server was created on 42Seoul"));
        SendMessage(IRCNumericMessage(RPL_MYINFO, HOSTNAME, "ft_irc(smun,seungyel,yejsong)-C++98-macOS", "o", ""));
        SendMessage(IRCNumericMessage(RPL_ISUPPORT, "PREFIX=(ov)@+", "CHANTYPES=#&", "CASEMAPPING=strict-rfc1459", "are supported by this server"));
        _server->OnMOTD(*this);

        if (GetRemoteAddress() == "127.0.0.1")
        {
            SendMessage(IRCMessage(HOSTNAME, "NOTICE", GetNickname(), "127.0.0.1 에서 접속되었습니다. 서버 관리자 모드를 활성화합니다."));
            SendMessage(IRCMessage(HOSTNAME, "MODE", GetNickname(), "+o"));
            _flag |= FLAG_OP;
        }

        SendMessage(IRCMessage(BOTNAME"!"BOTNAME"@"HOSTNAME, "NOTICE", GetNickname(), "봇 명령어를 보시려면  /msg "BOTNAME" !help  명령을 입력하세요."));
	}
}

int    IRCSession::HasOperatorFlag(std::string str)
{
	if (str.find("+o") != std::string::npos)
		return(1);
	if (str.find("-o") != std::string::npos)
		return(-1);
	return (0);
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

    // 숫자 응답일 경우,
    if (msg.IsNumeric())
    {
        // USER 및 NICK이 모두 등록되었다면 맨 앞 파라미터는 닉네임이 됨
        if (IsFullyRegistered())
            oss << " " << GetNickname();

        // 둘 중 하나라도 등록되지 않았다면 맨 앞 파라미터는 *이 됨.
        else
            oss << " " << "*";
    }

    // 다음, 각 param 추가.
    IRCMessage::ParamVectorConstIterator it;
    for (it = msg.BeginParam(); it != msg.EndParam(); ++it)
    {
        if (it->find_first_of(" :") != std::string::npos)
        {
            oss << " :" << *it;
            break;
        }
        else
            oss << " " << *it;
    }

    // 만들어진 최종 메시지를 전송.
    Send(oss.str());
}

void    IRCSession::MessageToNeighbor(const IRCMessage& msg, IRCSession* except)
{
    std::set<IRCSession*> neighbors;

    std::vector<const std::string>::iterator it;
    std::vector<const std::string> channels(_channels.begin(), _channels.end());
    for (it = channels.begin(); it != channels.end(); ++it)
        _server->GatherNeighbors(neighbors, _channels.begin(), _channels.end(), except);

    std::set<IRCSession*>::iterator nit = neighbors.begin();
    for (; nit != neighbors.end(); ++nit)
        (*nit)->SendMessage(msg);
}

void    IRCSession::CheckActive()
{
    const std::time_t curTime = std::time(NULL);

    if (_pingState == PingState_Active)
    {
        if (curTime - _lastPingTime > PINGTIMEDOUT)
        {
            Log::Vp("IRCSession::CheckActive", "PING을 전송했습니다.");
            _lastPingTime = curTime;
            _lastPingWord = HOSTNAME;
            SendMessage(IRCMessage("", "PING", HOSTNAME));
            _pingState = PingState_Idle;
        }
    }
    else
    {
        time_t diff = curTime - _lastPingTime;
        if (curTime - _lastPingTime > PINGTIMEDOUT)
        {
            Log::Vp("IRCSession::CheckActive", "핑 전송 후, 대기 시간을 초과하여 연결을 종료합니다.");
            Close("Ping timed out (" + String::ItoString(diff) + " secs over)");
        }
    }
}

void    IRCSession::OnPing(const IRCMessage& msg)
{
    if (msg.SizeParam() == 0)
        throw irc_exception(ERR_NEEDMOREPARAMS, "PING", "Not enough parameters.");
    SendMessage(IRCMessage(HOSTNAME, "PONG", HOSTNAME, msg.GetParam(0)));
}

void    IRCSession::OnPong(const IRCMessage& msg)
{
    if (msg.SizeParam() == 0)
        return;
    if (msg.GetParam(0) != _lastPingWord)
    {
        // ERR_WRONGPONG  reply?
        Log::Vp("IRCSession::OnPong", "수신 받은 값과, 클라이언트로 전송한 팡 값이 달라 퐁을 거절합니다. (서버:%s, 클라:%s)", _lastPingWord.c_str(), msg.GetParam(0).c_str());
        return;
    }
    Log::Vp("IRCSession::OnPong", "퐁을 수신하였습니다.");
    UpdateActive();
}

void    IRCSession::UpdateActive()
{
    _lastPingTime = std::time(NULL);
    _pingState = PingState_Active;
    _lastPingWord.clear();
    Log::Vp("IRCSession::UpdateActive", "마지막 패킷 수신 시각을 갱신합니다."); // 세션 유지를 위해.
}
