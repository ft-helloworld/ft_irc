/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   modelist.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/07 21:01:17 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 22:09:45 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODELIST_HPP
#define MODELIST_HPP

#include <string>
#include <vector>

class IRCMessage;

struct ModeChange
{
    enum { ERROR_SUCCESS, ERROR_UNKNOWNFLAG, ERROR_LACKPARAM };
    enum { CHANGEMODE_SUCCESS, CHANGEMODE_NOSUCHNICK, CHANGEMODE_NOTAFFECTED };

    int         sign;
    int         ch;
    std::string target;
    int         err;

    ModeChange(int err)
        : sign(), ch(), target(), err(err) {}

    ModeChange(int sign, int ch)
        : sign(sign), ch(ch), target(), err() {}

    ModeChange(int sign, int ch, const std::string& target)
        : sign(sign), ch(ch), target(target), err() {}

    inline ModeChange& SetChar(int ch) { this->ch = ch; return *this; }
};

struct ModeList
{
    typedef std::vector<ModeChange> ModeChangeVector;

    ModeChangeVector requestedList;
    ModeChangeVector changedList;

    const std::string ToString() const;

    static const ModeList Parse(
        const std::string& flags,
        const std::string& regularFlags,
        const std::string& paramFlags,
        const IRCMessage& msg,
        size_t msgParamIndex);
};

#endif
