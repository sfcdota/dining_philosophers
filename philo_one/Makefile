# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cbach <cbach@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/02/11 13:22:53 by cbach             #+#    #+#              #
#    Updated: 2021/02/11 13:22:56 by cbach            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME =	philo_one
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
	valgrind --leak-check=full --show-leak-kinds=all -s --track-fds=yes --track-origins=yes ./$(NAME) $(ARGS)
