# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/24 12:21:35 by smun              #+#    #+#              #
#    Updated: 2022/04/08 13:36:13 by smun             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98#-DLOG_LEVEL=5 -g -fsanitize=address #-O3
RM = rm
RMFLAGS = -f

SRCS = main ircsession ircsessionfactory irc_exception ircserver ircstring \
		ircmessage ircnumericmessage irccomparer ircchannel ircbot modelist
INC = -I./libserv/includes -I./includes

LIBSERV_SRCS = ./libserv/srcs/context ./libserv/srcs/channel ./libserv/srcs/log \
				./libserv/srcs/session ./libserv/srcs/string
FINAL_SRCS = $(addsuffix .cpp, $(addprefix srcs/, $(SRCS)) $(LIBSERV_SRCS))
FINAL_OBJS = $(FINAL_SRCS:.cpp=.o)

NAME = ircserv

all: $(NAME)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(INC) -c $< -o $@

$(NAME): $(FINAL_OBJS)
ifneq ($(CXX), c++)
	@echo "\033[33mWarning: \033[31mYour compiler is currently not c++. (current: "$(CXX)")\033[0m"
endif
ifneq ($(CPPFLAGS), -Wall -Wextra -Werror -std=c++98)
	@echo "\033[33mWarning: \033[31mYour compiler flags is not adequate. (current: "$(CPPFLAGS)")\033[0m"
endif
	$(CXX) $(CPPFLAGS) -o $@ $^

clean:
	$(RM) $(RMFLAGS) $(FINAL_OBJS)

fclean: clean
	$(RM) $(RMFLAGS) $(NAME)

re: clean all

.PHONY: all clean fclean re
