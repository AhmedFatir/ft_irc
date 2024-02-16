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

int ircsock;

class Bot
{
    private:
        std::vector<Player> players;
    public:
        void init();
        Bot();
        Player *GetPlayer(std::string nickname);
        ~Bot();
};

Bot::Bot(/* args */)
{
}

Bot::~Bot()
{
}

Player *Bot::GetPlayer(std::string nickname)
{	
    for (size_t i = 0; i < this->players.size(); i++){
		if (this->players[i].getNickname() == nickname)
			return &this->players[i];
	}
	return NULL;

}

void getCommand(std::string recived, std::string& nick , std::string &command)
{
	if(recived[0] == ':')
		recived.erase(recived.begin());
	size_t pos = recived.find('!');
	nick = recived.substr(0, pos);
	pos = std::string::npos;
	pos = recived.find(':');
	command = recived.substr(pos);
	if(!command.empty() && command[0] == ':')
		command.erase(command.begin());
}

void _sendMessage(std::string message, int fd)
{
    if(send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Send failed" << std::endl;
}


void send_privmsg(std::string message, std::string UserNick)
{
    std::string msg = "PRIVMSG " + UserNick + " :" + message + "\r\n";
    if (send(ircsock, msg.c_str(), msg.size(),0) == -1)
		std::cerr << "Send failed" << std::endl;
}

void drawBoard(const std::vector<char>& board, std::string UserNick)
{
	std::stringstream stm;
	stm << "-----------" << "\n";
	for (int i = 0; i < 9; ++i){
		if (board[i] == 'X')
			stm << " " << board[i] << " ";
		else if (board[i] == 'O')
			stm << " " << board[i] << " ";
		else
			stm << " " << board[i] << " ";

		if (i == 2 || i == 5)
			stm << "\n" << "-----------" << "\n";
		else if (i == 8)
			stm << "\n";
		else
			stm << "|";
	}
	stm << "-----------" << "\n";
	std::string line;
	while(std::getline(stm, line))
		{send_privmsg(line, UserNick); line.clear();}
}

// Function to check if a player has won
bool checkWin(const std::vector<char>& board, char player)
{
	// Check rows
	for (int i = 0; i <= 6; i += 3)
		{if (board[i] == player && board[i+1] == player && board[i+2] == player) return true;}
	// Check columns
	for (int i = 0; i < 3; i++)
		{if (board[i] == player && board[i+3] == player && board[i+6] == player) return true;}
	// Check diagonals
	if ((board[0] == player && board[4] == player && board[8] == player) ||
		(board[2] == player && board[4] == player && board[6] == player))
		return true;

	return false;
}

// std::string SplitBuff(std::string buff)
// {
// 	std::string command;
// 	size_t found = buff.find("bot");
// 	if (found != std::string::npos)
// 		command = buff.substr(found+4);
// 	else return "";
// 	found = command.find("\r\n");
// 	if (found != std::string::npos)
// 		command = command.substr(0, found);
// 	if (command[0] == ':')
// 		command.erase(command.begin());
// 	return command;
// }
// std::string SplitBuff(std::string buff, std::string &UserNick, std::string &date)
// {
// 	std::istringstream stm(buff);
// 	std::string token;
// 	stm >> UserNick;
// 	stm >> token;
// 	stm >> date;
// 	return token;
// }
// // Function to play the Tic-Tac-Toe game against the computer
// int GetNumber(std::string prompt, int ircsock, std::string UserNick)
// {
// 	ssize_t recivedBytes;
//     char buff[1024];

//   	std::string	command;
// 	while (1){
// 		send_privmsg(prompt, ircsock, UserNick);
// 		memset(buff, 0, sizeof(buff));
// 		if ((recivedBytes = recv(ircsock, buff, sizeof(buff) - 1, 0)) > 0){
// 			command = SplitBuff(buff);
// 			if (command == "exit") return -2;
// 			if(command.empty() || command.size() > 1 || !isdigit(command[0]))
// 				return -1;
// 			else break;
// 		}
// 		else
// 			return -2;
// 	}
// 	return (std::atoi(command.c_str()));
// }
int PotentialAction(const std::vector<char>& board, char player)
{
	// Check rows
	for (int i = 0; i <= 6; i += 3){
		if (board[i] == '-' && board[i+1] == player && board[i+2] == player) return i;
		if (board[i] == player && board[i+1] == '-' && board[i+2] == player) return i+1;
		if (board[i] == player && board[i+1] == player && board[i+2] == '-') return i+2;
	}
	// Check columns
	for (int i = 0; i < 3; i++){
		if (board[i] == '-' && board[i+3] == player && board[i+6] == player) return i;
		if (board[i] == player && board[i+3] == '-' && board[i+6] == player) return i+3;
		if (board[i] == player && board[i+3] == player && board[i+6] == '-') return i+6;
	}
	// Check diagonals
	if (board[0] == '-' && board[4] == player && board[8] == player) return 0;
	if (board[0] == player && board[4] == '-' && board[8] == player) return 4;
	if (board[0] == player && board[4] == player && board[8] == '-') return 8;

	if (board[2] == '-' && board[4] == player && board[6] == player) return 2;
	if (board[2] == player && board[4] == '-' && board[6] == player) return 4;
	if (board[2] == player && board[4] == player && board[6] == '-') return 6;

	return -1;
}

// void playTicTacToe(int ircsock, std::string UserNick)
// {
// 	std::vector<char> board(9, '-');
// 	send_privmsg("Welcome to (X | O) Game!", ircsock, UserNick);
// 	send_privmsg("YOU : X | Computer: O", ircsock, UserNick);
	
	
// 	char Player = 'X';
// 	int movesLeft = 9;
// 	while (movesLeft > 0)
// 	{
// 		drawBoard(board, ircsock, UserNick);

// 		if (Player == 'X'){ // Player's turn
// 			int move = GetNumber("To Quit Send (exit)/ YOU, enter your move (1-9):", ircsock, UserNick);
// 			if (move == -2) return;
// 			if (move == -1 || board[move - 1] != '-'){// Validate the move
// 				send_privmsg("Invalid move. Try again!", ircsock, UserNick);
// 				continue;
// 			}
// 			board[move - 1] = Player;
// 		}
// 		else{ // Computer's turn
// 			send_privmsg("Computer's turn ...", ircsock, UserNick);
// 			sleep(1);
// 			SetMove(board, Player, movesLeft);
// 		}
// 		if (checkWin(board, Player)){ // Check who wins
// 			drawBoard(board, ircsock, UserNick);
// 			if (Player == 'X')
// 				send_privmsg("YOU win!😁", ircsock, UserNick);
// 			else
// 				send_privmsg("Computer wins!🤖", ircsock, UserNick);
// 			return;
// 		}
// 		// Switch players
// 		if (Player == 'X') Player = 'O';
// 		else Player = 'X';
// 		movesLeft--;
// 	}
// 	drawBoard(board, ircsock, UserNick);
// 	send_privmsg("It's a draw!🙂", ircsock, UserNick);
// }

void	SetMove(std::vector<char>& board, char Player, int move)
{
	std::srand((std::time(NULL)));
	int PMove = -1;
	if (move == 6){ // always block
		PMove = PotentialAction(board, 'X');
		if (PMove != -1) board[PMove] = Player;
	}
	if (move == 2 || move == 4){ //try to win
		PMove = PotentialAction(board, 'O');
		if (PMove != -1) board[PMove] = Player;
	}
	if ((move == 2 || move == 4) && PMove == -1){ //if can't win, try to block
		int rand = std::rand() % 2; // randomize the move
		if (rand == 0) PMove = PotentialAction(board, 'X'); // block 50% of the time
		if (PMove != -1) board[PMove] = Player;
	}
	if (PMove == -1){ // if can't win or block, make a random move
		while (1){
			PMove = std::rand() % 9;
			if (board[PMove] == '-')
				{board[PMove] = Player; break;}
		}
	}
}


void Bot::init()
{
    std::string recived, nickname, command;
    ssize_t recivedBytes;

    char buff[1024];
	while(true)
    {
		memset(buff, 0, sizeof(buff));
		recivedBytes = recv(ircsock, buff, (sizeof(buff) - 1), 0);
		if(recivedBytes <= 0)
			break;
		recived = buff;
		std::cout << "Recived: "<< recived;
		size_t pos = recived.find_first_of("\n\r");
		if(pos != std::string::npos)
			recived = recived.substr(0, pos);
		if(recived.find("PRIVMSG") != std::string::npos)
		{
			getCommand(recived, nickname, command);
			Player *plyr = GetPlayer(nickname);
			if(command == "play" && !plyr)
            {
                Player player;
            	std::vector<char> board(9, '-');
                player.board = board;
                player.setIsPlaying(true);
                player.setNickname(nickname);
                players.push_back(player);
                send_privmsg("Welcome to (X | O) Game!",  nickname);
	            send_privmsg("YOU : X | Computer: O", nickname);
                drawBoard(player.board, nickname);
            } 
			else if (command == "play" && plyr && !plyr->getIsPlaying())
			{
                send_privmsg("Welcome to (X | O) Game!",  nickname);
	            send_privmsg("YOU : X | Computer: O", nickname);
				std::vector<char> board(9, '-');
				plyr->restMoves();
                plyr->board = board;
				drawBoard(plyr->board, nickname);
				plyr->setIsPlaying(true);
			}
            else if (plyr && plyr->getIsPlaying())
            {
				if(command.empty() || command.size() > 1 || !isdigit(command[0]) || plyr->getChatAtPos(std::atoi(command.c_str()) - 1) != '-' )
				{
                    send_privmsg("Invalid move. Try again!", nickname);
                	drawBoard(plyr->board, nickname);
                    continue;
				}
                int move = std::atoi(command.c_str());
                plyr->setChatAtPos(move - 1, 'X');
                drawBoard(plyr->board, nickname);
				plyr->dicMoves();
				if (checkWin(plyr->board, 'X') || !plyr->getMoves())
				{
					if(!plyr->getMoves())
						send_privmsg("It's a draw!🙂", nickname);
					else
						send_privmsg("YOU win!😁", nickname);
					plyr->setIsPlaying(false);
					plyr->board.clear();
					continue;
				}
                send_privmsg("Computer's turn ...", nickname);
			    sleep(1);
			    SetMove(plyr->board, 'O', plyr->getMoves());
                drawBoard(plyr->board, nickname);
				if (checkWin(plyr->board, 'O') || !plyr->getMoves()){// Check who wins
					if(!plyr->getMoves())
						send_privmsg("It's a draw!🙂", nickname);
					else
						send_privmsg("Computer wins!🤖", nickname);
					plyr->setIsPlaying(false);
					plyr->board.clear();
					continue;
				}
				plyr->dicMoves();
				std::cout << plyr->getMoves() << std::endl;
            }
			else
				send_privmsg("Invalid command. Try again!", nickname);
			// std::cout << "nickname:" << nickname << std::endl;
			// std::cout << "command:" << command << std::endl;
		}
		// if(std::strncmp(recived.c_str(), "PRIVMSG " , std::strlen("PRIVMSG ")))
		// {

		// }
        // buff[recivedBytes] = '\0';
        // recived = SplitBuff(buff, UserNick, date);
		// playTicTacToe(ircsock, UserNick);
		// _sendMessage("exit\r\n", ircsock);
	}
}


