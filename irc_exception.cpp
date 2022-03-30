/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_exception.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yejsong <yejsong@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 19:05:14 by yejsong           #+#    #+#             */
/*   Updated: 2022/03/30 19:22:35 by yejsong          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

irc_exception::~irc_exception() throw() {}

irc_exception::irc_exception(const irc_exception& o)
    : _message(o._message) {}

irc_exception::irc_exception(const std::string& errcode, const std::string& message) throw()
    : _message(std::string(":") + HOSTNAME + " " + errcode + " " + message) {}

const char* irc_exception::what() const throw()
{
    return _message.c_str();
}