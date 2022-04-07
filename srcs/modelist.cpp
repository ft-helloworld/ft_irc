/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   modelist.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/07 21:04:15 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 22:09:27 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "modelist.hpp"
#include "ircmessage.hpp"

const std::string ModeList::ToString() const
{
    std::string finale;

    int neg = 0;
    for (ModeChangeVector::const_iterator it = changedList.begin(); it != changedList.end(); ++it)
    {
        if (neg != it->sign)
            finale += static_cast<char>(neg = it->sign);
        finale += static_cast<char>(it->ch);
    }
    return finale;
}

const ModeList ModeList::Parse(
    const std::string& flags,
    const std::string& regularFlags,
    const std::string& paramFlags,
    const IRCMessage& msg,
    size_t msgParamIndex)
{
    ModeList ret;

    int sign = '+';
    std::string::const_iterator it = flags.begin();
    for (; it != flags.end(); ++it)
    {
        if (*it == '+' || *it == '-')
        {
            sign = *it;
            continue;
        }
        if (regularFlags.find(*it) != std::string::npos)
            ret.requestedList.push_back(ModeChange(sign, *it));
        else if (paramFlags.find(*it) != std::string::npos)
        {
            if (msgParamIndex >= msg.SizeParam())
                ret.requestedList.push_back(ModeChange(ModeChange::ERROR_LACKPARAM));
            else
                ret.requestedList.push_back(ModeChange(sign, *it, msg.GetParam(msgParamIndex++)));
        }
        else
            ret.requestedList.push_back(ModeChange(ModeChange::ERROR_UNKNOWNFLAG).SetChar(*it));
    }
    return ret;
}
