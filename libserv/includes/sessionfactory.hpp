/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sessionfactory.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 14:46:04 by smun              #+#    #+#             */
/*   Updated: 2022/03/30 14:53:35 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_FACTORY_HPP
#define SESSION_FACTORY_HPP

#include <iosfwd>

class Channel;
class Session;

class ISessionFactory
{
public:
    virtual Session*    CreateSession(Channel* channel, int socketfd, int socketId, const std::string& addr) = 0;
};

#endif
