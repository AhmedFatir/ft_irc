NAME = ircserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98  -g  -fsanitize=address,undefined
SRCS = Server.cpp Client.cpp Channel.cpp main.cpp  Mode.cpp \
		CMD/JOIN.cpp CMD/PART.cpp CMD/PRIVMSG.cpp CMD/KICK.cpp Authenti.cpp CMD/QUIT.cpp \
		CMD/TOPIC.cpp CMD/INVITE.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -g -o $(NAME) $(OBJS)

%.o: %.cpp Server.hpp Client.hpp Channel.hpp replies.hpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

khalid :
	make fclean
	make clean
	git add .
	git commit -m "auto commit khbouych"
	git push

.PHONY: all clean fclean re