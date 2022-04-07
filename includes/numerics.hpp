/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numerics.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 22:01:16 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 20:43:16 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NUMERICS_HPP
#define NUMERICS_HPP

#define RPL_WELCOME          1
#define RPL_YOURHOST         2
#define RPL_CREATED          3
#define RPL_MYINFO           4
#define RPL_ISUPPORT         5

#define RPL_TEXT             304
#define RPL_LISTSTART        321
#define RPL_LIST             322
#define RPL_LISTEND          323
#define RPL_CHANNELMODEIS    324
#define RPL_CREATIONTIME     329
#define RPL_NOTOPIC          331
#define RPL_TOPIC            332
#define RPL_TOPICWHOTIME     333
#define RPL_NAMREPLY         353
#define RPL_ENDOFNAMES       366
#define RPL_MOTD             372
#define RPL_MOTDSTART        375
#define RPL_ENDOFMOTD        376

#define ERR_NOSUCHNICK       401
#define ERR_NOSUCHCHANNEL    403
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_TOOMANYCHANNELS  405
#define ERR_NORECIPIENT      411
#define ERR_NOTEXTTOSEND     412
#define ERR_UNKNOWNCOMMAND   421
#define ERR_NONICKNAMEGIVEN  431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE    433
#define ERR_NOTONCHANNEL     442
#define ERR_NOTREGISTERED    451
#define ERR_NEEDMOREPARAMS   461
#define ERR_ALREADYREGISTRED 462
#define ERR_UNKNOWNMODE      472
#define ERR_BADCHANMASK      476
#define ERR_NOPRIVILEGES     481
#define ERR_CHANOPRIVSNEEDED 482

#endif
