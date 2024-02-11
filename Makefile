NAME_BOUNUS = bot
NAME = ircserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address,undefined

SRCS = Server.cpp Client.cpp Channel.cpp main.cpp  Mode.cpp \
		CMD/JOIN.cpp CMD/PART.cpp CMD/PRIVMSG.cpp CMD/KICK.cpp Authenti.cpp CMD/QUIT.cpp \
		CMD/TOPIC.cpp CMD/INVITE.cpp 
SRCS_BONUS = bot.cpp

OBJS = $(SRCS:.cpp=.o)
OBJS_BONUS = $(SRCS_BONUS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

bonus: $(OBJS_BONUS)
	@$(CC) $(CFLAGS) -o $(NAME_BOUNUS) $(OBJS_BONUS)

%.o: %.cpp Server.hpp Client.hpp Channel.hpp replies.hpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS) $(OBJS_BONUS)

fclean: clean
	@rm -f $(NAME) $(NAME_BOUNUS)

re: fclean all

khbouych :
	make fclean
	make clean
	git add .

.PHONY: all clean fclean re