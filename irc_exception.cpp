/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_exception.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 19:05:14 by yejsong           #+#    #+#             */
/*   Updated: 2022/03/31 01:53:46 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "irc_exception.hpp"

irc_exception::~irc_exception() throw() {}

irc_exception::irc_exception(const irc_exception& o)
    : _message(o._message) {}

irc_exception::irc_exception(int errcode, const std::string& message) throw()
    : _message(":"HOSTNAME" " + String::ItoCode(errcode) + " " + message) {}

const char* irc_exception::what() const throw()
{
    return _message.c_str();
}
