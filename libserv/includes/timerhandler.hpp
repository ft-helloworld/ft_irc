/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timerhandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/05 17:43:16 by smun              #+#    #+#             */
/*   Updated: 2022/04/05 20:38:44 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIMERHANDLER_HPP
#define TIMERHANDLER_HPP

class   ITimerHandler
{
public:
    virtual void    OnTimer() = 0;
    virtual size_t  GetInterval() const = 0;
};

#endif
