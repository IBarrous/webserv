NAME=server
FILES= main.cpp Client.cpp Server.cpp
CC= c++
CPPFLAGS= -Wall -Wextra -Werror -std=c++98 #-fsanitize=address
OBJ=${FILES:.cpp=.o}

all:$(NAME)

$(NAME) : ${OBJ}
	${CC} ${CPPFLAGS} ${OBJ} -o ${NAME}

clean:
	rm -rf ${OBJ}

fclean : clean
	rm -rf ${NAME}

re : fclean all
