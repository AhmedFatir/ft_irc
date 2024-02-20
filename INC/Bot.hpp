#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include <netdb.h>
#include <sstream>
#include <ctime>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <cstring>
#include "Player.hpp"

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

class Bot
{
private:
	std::vector<Player> players;
	std::vector<std::string> vnokat;
	bool Loggedin;
	std::string botnick;
public:
	Bot();
	Bot(const Bot& other);
	Bot& operator=(const Bot& other);
	~Bot();

	//init methods
	void	init(int ircsock);
	Player	*GetPlayer(std::string nickname);
	//send methods
	void	_sendMessage(std::string message, int fd);
	void	send_privmsg(std::string message, std::string UserNick, int ircsock);
	//nokat methods
	int		getnokat(std::string filename);
	std::string getnokta(std::vector<std::string> &vnokat, int size);
	//age methods
	int		ParsAge(std::string age);
	void	ageCalculator(std::string age, std::string Nickname,int ircsock);
	//parse methods
	void	getCommand(std::string recived, std::string& nick , std::string &command);
	std::string SplitBuff(std::string buff, std::string &date);
	//game methods
	void	drawBoard(const std::vector<char>& board, std::string UserNick, int ircsock);
	bool	checkWin(const std::vector<char>& board, char player);
	int		PotentialAction(const std::vector<char>& board, char player);
	void	SetMove(std::vector<char>& board, char Player, int move);
	void	setNick(std::string nick);
	void	PlayGame(std::string command, std::string nickname, int ircsock);
};
