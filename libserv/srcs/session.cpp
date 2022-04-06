/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:32:11 by smun              #+#    #+#             */
/*   Updated: 2022/04/06 15:44:05 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "std.hpp"
#include "channel.hpp"
#include "session.hpp"
#include "log.hpp"
#include "string.hpp"
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
    , _triggeredEvents(IOEvent_Read)
{
    Log::Vp("Session::Session", "[%d/%s] 세션 인스턴스를 생성합니다.", GetSocket(), GetRemoteAddress().c_str());
}

Session::~Session()
{
    if (_attachedChannel == NULL)
        return;
    if (_triggeredEvents)
    {
        _attachedChannel->SetEvent(GetSocket(), _triggeredEvents, IOFlag_Remove, NULL);
        _triggeredEvents = 0;
    }
    Log::Vp("Session::~Session", "[%d/%s] 세션 인스턴스가 삭제됩니다.", GetSocket(), GetRemoteAddress().c_str());
}

bool    Session::GetNextLine(ByteBuffer& buffer, std::string& line)
{
    const ByteBufferIterator begin = buffer.begin();
    const ByteBufferIterator end = buffer.end();
    ByteBufferIterator lineEnd;

    // \r\n 형태의 개행을 검사하여 분리.
    lineEnd = std::search(begin, end, CRLF, &CRLF[CRLF_SIZE]);
    if (lineEnd != end)
    {
        line.assign(begin, lineEnd);
        buffer.erase(begin, lineEnd + CRLF_SIZE);
        return true;
    }

    // \n 형태의 개행을 검사하여 분리.
    lineEnd = std::search(begin, end, LF, &LF[LF_SIZE]);
    if (lineEnd != end)
    {
        line.assign(begin, lineEnd);
        buffer.erase(begin, lineEnd + LF_SIZE);
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

    if (_recvBuffer.size() > MAX_MESSAGE_LEN)
    {
        Log::Fp("Session::OnRead", "Exceed recv buffer size %d (from %s)", MAX_MESSAGE_LEN, GetRemoteAddress().c_str());
        Close();
    }
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
        bytes = _sendBuffer.size(); // 버퍼에서 전부 꺼내기
    }
    TakeBuffer(static_cast<size_t>(bytes));
    if (_sendBuffer.empty())
    {
        Log::Vp("Session::OnWrite", "[%d/%s] 세션의 송신 버퍼가 비었습니다. 송신 IO 플래그를 해제합니다.", GetSocket(), GetRemoteAddress().c_str());
        DisableWriteEvent();
    }
}

void    Session::DisableWriteEvent()
{
    if (_triggeredEvents & IOEvent_Write)
    {
        _attachedChannel->SetEvent(GetSocket(), IOEvent_Write, IOFlag_Remove, this);
        _triggeredEvents &= ~IOEvent_Write;
        Log::Vp("Session::OnWrite", "[%d/%s] 세션의 송신 IO 플래그를 해제했습니다.", GetSocket(), GetRemoteAddress().c_str());
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
    if (_attachedChannel == NULL)
        return;
    if (!_closed)
    {
        Log::Vp("Session::Close", "[%d/%s] 세션의 닫기 이벤트를 트리거 합니다.", GetSocket(), GetRemoteAddress().c_str());
        if (_triggeredEvents & IOEvent_Read)
        {
            _attachedChannel->SetEvent(GetSocket(), IOEvent_Read, IOFlag_Remove, NULL);
            _triggeredEvents &= ~IOEvent_Read;
            Log::Vp("Session::Close", "[%d/%s] 세션의 읽기 이벤트를 제거합니다.", GetSocket(), GetRemoteAddress().c_str());
        }
        _attachedChannel->SetEvent(GetSocket(), IOEvent_Close, IOFlag_Add | IOFlag_OneShot, this);
        _closed = true;
    }
    else
        Log::Vp("Session::Close", "[%d/%s] 세션의 닫기 이벤트가 이미 트리거되었습니다.", GetSocket(), GetRemoteAddress().c_str());
}

void    Session::Process(const std::string& line)
{
    Log::Dp("Session::Process", "[R/%s] %s", _remoteAddress.c_str(), line.c_str());
}

void    Session::Send(const std::string& line)
{
    Log::Dp("Session::Send", "[S/%s] %s", _remoteAddress.c_str(), line.c_str());
    Send(&line[0], line.length());
}

void    Session::Send(const void* buf, size_t len)
{
    const Byte* const bytebuf = reinterpret_cast<const Byte*>(buf);

    // 소켓이 닫혔을 때는 송신 버퍼에 데이터를 작성하지 않음.
    if (_closed)
        return;
    _sendBuffer.insert(_sendBuffer.end(), bytebuf, bytebuf + std::min<size_t>(MAX_MESSAGE_LEN - CRLF_SIZE, len));
    _sendBuffer.insert(_sendBuffer.end(), CRLF, &CRLF[CRLF_SIZE]);
    if (!(_triggeredEvents & IOEvent_Write))
    {
        _triggeredEvents |= IOEvent_Write;
        _attachedChannel->SetEvent(GetSocket(), IOEvent_Write, IOFlag_Add | IOFlag_Enable, this);
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
