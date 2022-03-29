# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/24 12:21:35 by smun              #+#    #+#              #
#    Updated: 2022/03/29 18:18:24 by smun             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g
RM = rm
RMFLAGS = -f

SRCS = main
INC = -I./libserv/includes -I./

LIBSERV_SRCS = ./libserv/srcs/context ./libserv/srcs/channel ./libserv/srcs/log ./libserv/srcs/session
FINAL_SRCS = $(addsuffix .cpp, $(SRCS) $(LIBSERV_SRCS))
FINAL_OBJS = $(FINAL_SRCS:.cpp=.o)

NAME = ircserv

all: $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(NAME): $(FINAL_OBJS)
ifneq ($(CC), c++)
	@echo "\033[33mWarning: \033[31mYour compiler is currently not c++. (current: "$(CC)")\033[0m"
endif
ifneq ($(CFLAGS), -Wall -Wextra -Werror -std=c++98)
	@echo "\033[33mWarning: \033[31mYour compiler flags is not adequate. (current: "$(CFLAGS)")\033[0m"
endif
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(RMFLAGS) $(FINAL_OBJS)

fclean: clean
	$(RM) $(RMFLAGS) $(NAME)

re: clean all

.PHONY: all clean fclean re
