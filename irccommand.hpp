/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irccommand.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 16:44:30 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 16:53:22 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCCOMMAND_HPP
#define IRCCOMMAND_HPP

#include <string>
#include "string.hpp"

struct IRCCommand
{
    const std::string command;

    inline IRCCommand(int cmd)
        : command(String::ItoCode(cmd)) {}
    inline IRCCommand(const std::string& cmd)
        : command(cmd) {}
};

#endif
