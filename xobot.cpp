#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

void drawBoard(const std::vector<char>& board)
{
	std::cout << "-----------" << std::endl;
	for (int i = 0; i < 9; ++i){
		if (board[i] == 'X')
			std::cout << GRE << " " << board[i] << " " << WHI;
		else if (board[i] == 'O')
			std::cout << RED << " " << board[i] << " " << WHI;
		else
			std::cout << " " << board[i] << " ";

		if (i == 2 || i == 5)
			std::cout << std::endl << "-----------" << std::endl;
		else if (i == 8)
			std::cout << std::endl;
		else
			std::cout << "|";
	}
	std::cout << "-----------" << std::endl;
}

// Function to check if a player has won
bool checkWin(const std::vector<char>& board, char player)
{
	// Check rows
	for (int i = 0; i <= 6; i += 3){
		if (board[i] == player && board[i+1] == player && board[i+2] == player)
			return true;
	}
	// Check columns
	for (int i = 0; i < 3; i++){
		if (board[i] == player && board[i+3] == player && board[i+6] == player)
			return true;
	}
	// Check diagonals
	if ((board[0] == player && board[4] == player && board[8] == player) ||
		(board[2] == player && board[4] == player && board[6] == player))
		return true;

	return false;
}

// Function to play the Tic-Tac-Toe game against the computer
int GetNumber(std::string prompt)
{
  	std::string	command;
	int number;
	while (1){
		std::cout << prompt;
		if (std::getline(std::cin, command)){
			if(command.empty() || command.size() > 1 || !isdigit(command[0]))
				return -1;
			else break;
		}
		else{
			if (std::cin.eof())
				{std::cin.clear(); rewind(stdin); std::cout << std::endl;}
		}
	}
	return (std::atoi(command.c_str()));
}

void playTicTacToe()
{
	std::vector<char> board(9, '-');
	std::cout << YEL << "Welcome to (X | O) Game!" << WHI << std::endl;
	std::cout << "YOU : " << GRE << "X" << WHI << " | Computer: " << RED <<  "O" << WHI << std::endl;
	char currentPlayer = 'X';
	int movesLeft = 9;
	while (movesLeft > 0){
		drawBoard(board);

		if (currentPlayer == 'X'){ // Player's turn
			int move = GetNumber("YOU, enter your move (1-9): ");
			if (move == -1 || board[move - 1] != '-'){// Validate the move
				std::cout << RED << "Invalid move. Try again!" << WHI << std::endl;
				continue;
			}
			board[move - 1] = currentPlayer;
		}
		else{ // Computer's turn
			std::cout << "Computer's turn..." << std::endl;
			sleep(1);
			int computerMove;
			while (1){
				computerMove = std::rand() % 9;
				if (board[computerMove] == '-')
					{board[computerMove] = currentPlayer; break;}
			}
			board[computerMove] = currentPlayer;
		}
		if (checkWin(board, currentPlayer)){ // Check if the current player has won
			drawBoard(board);
			if (currentPlayer == 'X')
				std::cout << GRE << "YOU win!" << WHI << std::endl;
			else
				std::cout << RED << "Computer wins!" << WHI << std::endl;
			return;
		}
		// Switch players
		if(currentPlayer == 'X') currentPlayer = 'O';
		else currentPlayer = 'X';
		movesLeft--;
	}
	drawBoard(board);
	std::cout << YEL << "It's a draw!" << WHI << std::endl;
}

int main()
{
	while (1){
		system("clear");
		std::srand((std::time(NULL)));
		playTicTacToe();
		int i = GetNumber("Play again? if yes enter 1: ");
		if (i != 1)
			{std::cout << "Goodbye!" << std::endl; break;}
	}
	return 0;
}
