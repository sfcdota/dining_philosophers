# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cbach <cbach@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/02/11 13:10:17 by cbach             #+#    #+#              #
#    Updated: 2021/02/11 13:29:07 by cbach            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME =	philo_two
FLAGS = -Wall -Wextra -Werror -pthread

HEADERS =	$(wildcard ./*.h)

SRC = 		$(wildcard ./*.c)

OBJ = 		$(patsubst %.c, %.o, $(SRC))

.PHONY: all clean fclean re


all: $(NAME)
	@rm -f .depend


$(NAME): $(OBJ)
	gcc $(FLAGS) $(OBJ) -o $(NAME)

.depend: $(SRC) $(BONUS)
	@rm -f .depend
	gcc $(FLAGS) -MM $^ > .depend

-include .depend

%.c:
	gcc $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)
	@rm -f .depend

fclean: clean
	rm -f $(NAME)

re: fclean all

valgrind: ./$(NAME)
	valgrind --leak-check=full --show-leak-kinds=all -s --track-fds=yes ./$(NAME) $(ARGS)
