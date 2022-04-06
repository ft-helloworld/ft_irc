/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircmessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 13:45:47 by smun              #+#    #+#             */
/*   Updated: 2022/04/06 18:50:45 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>
#include "irccommand.hpp"

class IRCMessage
{
public:
    typedef std::vector<std::string> ParamVector;
    typedef std::vector<std::string>::const_iterator ParamVectorConstIterator;

private:
    const std::string   _prefix;
    const IRCCommand    _cmd;
    ParamVector         _params;

    IRCMessage& operator= (const IRCMessage&);

    static const IRCMessage Empty;

public:
    IRCMessage();
    virtual ~IRCMessage();
    IRCMessage(const IRCMessage& msg);

    IRCMessage(const IRCCommand& cmd);
    IRCMessage(const std::string& prefix, const IRCCommand& cmd);
    IRCMessage(const std::string& prefix, const IRCCommand& cmd, const std::string& p1);
    IRCMessage(const std::string& prefix, const IRCCommand& cmd, const std::string& p1, const std::string& p2);
    IRCMessage(const std::string& prefix, const IRCCommand& cmd, const std::string& p1, const std::string& p2, const std::string& p3);

    inline const std::string&          GetCommand() const  { return _cmd.command; }
    inline const std::string&          GetTrailing() const { return _params.back(); }
    inline const std::string&          GetPrefix() const   { return _prefix; }

    inline void                        AddParam(const std::string& param) { _params.push_back(param); }
    const std::string                  GetParams(ParamVector::size_type begin = 0, ParamVector::size_type end = SIZE_MAX) const;

    inline ParamVectorConstIterator    BeginParam() const  { return _params.begin(); }
    inline ParamVectorConstIterator    EndParam() const    { return _params.end(); }
    inline ParamVector::size_type      SizeParam() const   { return _params.size(); }

    /**
     * @brief 해당 인덱스의 파라미터 값을 가져옵니다. 0은 0번째 파라미터, 1은 1번째 파라미터, 2는 2번째 파라미터 등등 입니다.
     *
     * @param i 가져올 파라미터의 인덱스 입니다.
     * @return 해당 인덱스의 파라미터 문자열을 가져옵니다.
     */
    inline const std::string&          GetParam(ParamVector::size_type i) const { return _params[i]; }

    /**
     * @brief 명령어가 비어있는 메시지인지 여부를 나타냅니다.
     * 일반적으로 클라이언트에서 빈 줄을 보냈을 경우, 이 값이 참이 됩니다.
     *
     * @return true 는 명령어가 비어있으면 반환됩니다.
     * false 는 명령어가 비어있지 않으면 반환됩니다.
     */
    inline bool    IsEmpty() const { return _cmd.command.empty(); }

    /**
     * @brief 숫자로 이루어진 명령어를 가진 메시지인지 여부를 나타냅니다.
     * IRCMessage 클래스 생성자의 IRCCommand 인자가 숫자에 의해 생성되었을 경우, 이 값이 참이 됩니다.
     *
     * @return true 는 숫자로 이루어진 명령어를 가진 메시지일 경우 반환됩니다.
     * false 는 숫자로 이루어진 명령어가 이날 경우 반환됩니다.
     */
    inline bool    IsNumeric() const { return _cmd.numeric; }

    /**
     * @brief IRC 메시지 포맷의 줄에서 Prefix, Command, Params, Trailing 요소들을 모두 파싱합니다.
     *
     * @param line 파싱할 IRC 메시지 포맷을 가진 문자열입니다.
     * @return IRCMessage 파싱된 IRCMessage 객체를 반환합니다.
     */
    static IRCMessage Parse(const std::string& line);
};

#endif
