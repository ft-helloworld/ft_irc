/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   moderesult.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/07 17:04:32 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 17:05:45 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODERESULT_HPP
#define MODERESULT_HPP

#include <vector>
#include <string>

struct ModeChange
{
    int sign;
    int ch;
    ModeChange(int sign, int ch)
        : sign(sign), ch(ch) {}
};

struct  ModeResult
{
    std::vector<ModeChange> changedFlags;
    std::vector<const std::string> arguments;
};

#endif
