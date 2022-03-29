/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:32:11 by smun              #+#    #+#             */
/*   Updated: 2022/03/29 19:20:20 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "std.hpp"
#include "channel.hpp"
#include "session.hpp"
#include "log.hpp"
#include <algorithm>
#include <sys/socket.h>
#include <cstring>
#include <string>
#include <cerrno>
#include <memory>
#include <sstream>
#include <iostream>
#include <vector>

Session::Session(Channel* channel, int socketfd, int socketId, const std::string& addr)
    : Context(channel, socketfd)
    , _socketId(socketId)
    , _remoteAddress(addr)
    , _recvBuffer()
    , _sendBuffer()
    , _triggeredEvents(0)
{
    Log::Vp("Session::Session", "[%d/%s] 세션 인스턴스를 생성합니다.", GetSocket(), GetRemoteAddress().c_str());
}

Session::~Session()
{
    if (_triggeredEvents)
        _attachedChannel->SetEvent(GetSocket(), _triggeredEvents, IOFlag_Remove, NULL);
    _triggeredEvents = 0;
    Log::Vp("Session::~Session", "[%d/%s] 세션 인스턴스가 삭제됩니다.", GetSocket(), GetRemoteAddress().c_str());
}

bool    Session::GetNextLine(ByteBuffer& buffer, std::string& line)
{
    const ByteBufferIterator begin = buffer.begin();
    const ByteBufferIterator end = buffer.end();
    ByteBufferIterator lineEnd;

    lineEnd = std::search(begin, end, CRLF, &CRLF[CRLF_SIZE]);
    if (lineEnd != end)
    {
        line.assign(begin, lineEnd);
        buffer.erase(begin, lineEnd + CRLF_SIZE);
        return true;
    }
    return false;
}

void    Session::OnRead()
{
    Byte buffer[TCP_MTU];

    ssize_t bytes = recv(GetSocket(), buffer, sizeof(buffer), 0);
    if (bytes <= 0)
    {
        if (bytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return;
        Log::Dp("Session::OnRead", "[%d/%s] 원격 연결이 끊어졌습니다. 세션 종료를 시작합니다. errno:%d", GetSocket(), GetRemoteAddress().c_str(), errno);
        Close();
        return;
    }
    AppendBuffer(buffer, static_cast<size_t>(bytes));

    std::string line;
    while (GetNextLine(_recvBuffer, line))
        Process(line);
}

void    Session::AppendBuffer(Byte* buffer, size_t bytes)
{
    _recvBuffer.reserve(_recvBuffer.size() + bytes);
    _recvBuffer.insert(_recvBuffer.end(), &buffer[0], &buffer[bytes]);
    Log::Vp("Session::AppendBuffer", "[%d/%s] 세션의 수신 버퍼에 %llu 바이트를 더했습니다. (현재 총 바이트: %llu)", GetSocket(), GetRemoteAddress().c_str(), bytes, _recvBuffer.size());
}

void    Session::OnWrite()
{
    ssize_t bytes = send(GetSocket(), _sendBuffer.data(), _sendBuffer.size(), 0);
    if (bytes < 0)
    {
        if (bytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return;
        Log::Dp("Session::OnWrite", "[%d/%s] 원격 연결이 끊어졌습니다. 세션 종료를 시작합니다. errno:%d", GetSocket(), GetRemoteAddress().c_str(), errno);
        Close();
        return;
    }
    TakeBuffer(static_cast<size_t>(bytes));
    if (_sendBuffer.empty())
    {
        Log::Vp("Session::OnWrite", "[%d/%s] 세션의 송신 버퍼가 비었습니다. 송신 IO 플래그를 해제합니다.", GetSocket(), GetRemoteAddress().c_str());
        _attachedChannel->SetEvent(GetSocket(), IOEvent_Write, IOFlag_Disable, this);
        _triggeredEvents &= ~IOEvent_Write;
    }
}

void    Session::TakeBuffer(size_t bytes)
{
    const ByteBufferIterator begin  = _sendBuffer.begin();
    const ByteBufferIterator end    = _sendBuffer.begin() + bytes;

    _sendBuffer.erase(begin, end);
    Log::Vp("Session::TakeBuffer", "[%d/%s] 세션의 송신 버퍼에서 %llu 바이트를 꺼냈습니다. (현재 남은 바이트: %llu)", GetSocket(), GetRemoteAddress().c_str(), bytes, _sendBuffer.size());
}

void    Session::Close()
{
    _attachedChannel->Close(this);
}

static void split_arguments(std::vector<const std::string>& args, const std::string& line)
{
    std::istringstream iss(line);
    std::string s;
    while (std::getline(iss, s, ' '))
        if (!s.empty())
            args.push_back(s);
}

static int    mini_stoi(const std::string& str)
{
    std::istringstream ss(str);
    int n;
    ss >> n;
    if (ss.fail())
        throw std::runtime_error("Not integer");
    return n;
}

static std::string join_strings(
    std::vector<const std::string>::const_iterator begin,
    std::vector<const std::string>::const_iterator end)
{
    std::ostringstream oss;
    while (begin != end)
    {
        oss << *begin;
        ++begin;
        if (begin != end)
            oss << ' ';
    }
    return oss.str();
}

void    Session::Process(const std::string& line)
{
    Log::Ip("Session::Process", "[R/%s] %s", _remoteAddress.c_str(), line.c_str());

    // 날아온 한 줄 처리
    std::vector<const std::string> args;

    // 한 줄에서 스페이스 문자로 구분
    split_arguments(args, line);
    if (args.size() == 0)
        return;

    // 명령어 처리
    if (args[0] == "HELLO")
    {
        try
        {
            if (args.size() < 2)
                throw std::runtime_error("No parameter with HELLO.");
            Send("HELLO Your name is " + args[1] + "!! Welcome to my server :)");
        }
        catch (const std::exception& ex)
        {
            Send(ex.what());
        }
    }
    else if (args[0] == "MESSAGE")
    {
        try
        {
            if (args.size() < 3)
                throw std::runtime_error("Lacked parameter with MESSAGE.");
            int targetId = mini_stoi(args[1]);
            std::string message = join_strings(args.begin() + 2, args.end());
            Session& target = _attachedChannel->FindSession(targetId);
            target.Send("MESSAGE " + message);
        }
        catch (const std::exception& ex)
        {
            Send(ex.what());
        }
    }
    else
    {
        Send("Unknown command " + args[0]);
    }
}

void    Session::Send(const std::string& line)
{
    Log::Ip("Session::Send", "[S/%s] %s", _remoteAddress.c_str(), line.c_str());
    Send(&line[0], line.length());
}

void    Session::Send(const void* buf, size_t len)
{
    const Byte* const bytebuf = reinterpret_cast<const Byte*>(buf);

    _sendBuffer.insert(_sendBuffer.end(), bytebuf, bytebuf + len);
    _sendBuffer.insert(_sendBuffer.end(), CRLF, &CRLF[CRLF_SIZE]);
    if (!(_triggeredEvents & IOEvent_Write))
    {
        _triggeredEvents |= IOEvent_Write;
        _attachedChannel->SetEvent(GetSocket(), IOEvent_Write, IOFlag_Enable, this);
    }
}

const std::string&  Session::GetRemoteAddress() const
{
    return _remoteAddress;
}

int Session::GetSocketId() const
{
    return _socketId;
}
