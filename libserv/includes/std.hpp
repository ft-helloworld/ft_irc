/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   std.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 12:25:54 by smun              #+#    #+#             */
/*   Updated: 2022/04/01 16:40:07 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STD_HPP
#define STD_HPP

#include <cstddef>
#include <iosfwd>

// data buffer
#define TCP_MTU (1500 - 20 - 20)

#define MAX_MESSAGE_LEN 512

// line sperator (http/irc)
#define CRLF "\r\n"
#define CRLF_SIZE sizeof(CRLF) - 1

#define LF "\n"
#define LF_SIZE sizeof(LF) - 1

enum { IOEvent_Read = 1 << 0, IOEvent_Write = 1 << 1, IOEvent_Close = 1 << 2 };
enum { IOFlag_Add = 1 << 0, IOFlag_Remove = 1 << 1, IOFlag_Enable = 1 << 2, IOFlag_Disable = 1 << 3, IOFlag_OneShot = 1 << 4 };

typedef unsigned char	Byte;

#endif
