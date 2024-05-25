NAME_BOUNUS = bot
NAME = ircserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp SRC/Authenti.cpp SRC/Channel.cpp SRC/Client.cpp SRC/Server.cpp \
	CMD/INVITE.cpp CMD/JOIN.cpp CMD/KICK.cpp CMD/MODE.cpp CMD/PART.cpp CMD/PRIVMSG.cpp CMD/QUIT.cpp CMD/TOPIC.cpp 

SRCS_BONUS = BONUS/main.cpp BONUS/player.cpp BONUS/bot.cpp

OBJS = $(SRCS:.cpp=.o)
OBJS_BONUS = $(SRCS_BONUS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

bonus: $(OBJS_BONUS)
	@$(CC) $(CFLAGS) -o $(NAME_BOUNUS) $(OBJS_BONUS)

%.o: %.cpp INC/Server.hpp INC/Client.hpp INC/Channel.hpp INC/replies.hpp INC/Bot.hpp INC/Player.hpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS) $(OBJS_BONUS)

fclean: clean
	@rm -f $(NAME) $(NAME_BOUNUS) a.out

re: fclean all

.PHONY: all clean fclean re