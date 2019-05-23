NAME = my_ping

SRC = $(addprefix src/, main.c echo.c icmp_socket.c)

CC = gcc

DEBUG = -fsanitize=address -fsanitize=undefined
INCLUDES = $(addprefix -I, ./src)
CFLAGS += -Wall -Wextra -Werror $(DEBUG) $(INCLUDES)

DOCKER_MACHINE_NAME = ping

###

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

dm-create:
	docker-machine create --driver virtualbox $(DOCKER_MACHINE_NAME)

dm-start:
	docker-machine start $(DOCKER_MACHINE_NAME)
	@echo docker-machine env $(DOCKER_MACHINE_NAME)

run:
	docker run -it debian

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all