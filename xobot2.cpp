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
#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

void send_privmsg(int srvsock,std::string age, std::string nickname)
{
    std::string msg = "PRIVMSG " +nickname + " :your age is: " +age +"\r\n";
    send(srvsock, msg.c_str(), msg.size(),0);
}

void _sendMessage(std::string message, int fd)
{
    if(send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Send failed" << std::endl;
}

void drawBoard(const std::vector<char>& board, int ircsock)
{
	std::stringstream stm;
	stm << "-----------" << std::endl;
	for (int i = 0; i < 9; ++i){
		if (board[i] == 'X')
			stm << GRE << " " << board[i] << " " << WHI;
		else if (board[i] == 'O')
			stm << RED << " " << board[i] << " " << WHI;
		else
			stm << " " << board[i] << " ";

		if (i == 2 || i == 5)
			stm << std::endl << "-----------" << std::endl;
		else if (i == 8)
			stm << std::endl;
		else
			stm << "|";
	}
	stm << "-----------" << "\n\r";
	_sendMessage(stm.str(), ircsock);
	std::cout << stm.str() << std::endl;
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
int GetNumber(std::string prompt, int ircsock)
{
  	std::string	command;
	while (1){
		_sendMessage(prompt, ircsock);
		if (std::getline(std::cin, command)){
			if(command.empty() || command.size() > 1 || !isdigit(command[0]))
				return -1;
			else break;
		}
		// else{
		// 	if (std::cin.eof())
		// 		{std::cin.clear(); rewind(stdin); std::cout << std::endl;}
		// }
	}
	return (std::atoi(command.c_str()));
}

void playTicTacToe(int ircsock)
{
	std::vector<char> board(9, '-');
	std::stringstream stm;
	stm << YEL << "Welcome to (X | O) Game!" << WHI << std::endl;
	stm << "YOU : " << GRE << "X" << WHI << " | Computer: " << RED <<  "O" << WHI << "\n\r";
	_sendMessage(stm.str(), ircsock);
	std::cout << stm.str() << std::endl;
	
	
	char currentPlayer = 'X';
	int movesLeft = 9;
	while (movesLeft > 0){
		drawBoard(board, ircsock);

		if (currentPlayer == 'X'){ // Player's turn
			int move = GetNumber("YOU, enter your move (1-9): \n\r", ircsock);
			if (move == -1 || board[move - 1] != '-'){// Validate the move
				stm.str("");
				stm << RED << "Invalid move. Try again!" << WHI << "\n\r";
				_sendMessage(stm.str(), ircsock);
				std::cout << RED << "Invalid move. Try again!" << WHI << std::endl;
				continue;
			}
			board[move - 1] = currentPlayer;
		}
		else{ // Computer's turn
			stm.str("");
			stm << "Computer's turn..." << "\n\r";
			_sendMessage(stm.str(), ircsock);
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
			drawBoard(board, ircsock);
			if (currentPlayer == 'X'){
				stm.str("");
				stm << GRE << "YOU win!" << WHI << "\n\r";
				_sendMessage(stm.str(), ircsock);
				std::cout << GRE << "YOU win!" << WHI << std::endl;
			}
			else{
				stm.str("");
				stm << RED << "Computer wins!" << WHI << "\n\r";
				_sendMessage(stm.str(), ircsock);
				std::cout << RED << "Computer wins!" << WHI << std::endl;
			}
			return;
		}
		// Switch players
		if(currentPlayer == 'X') currentPlayer = 'O';
		else currentPlayer = 'X';
		movesLeft--;
	}
	drawBoard(board, ircsock);
	stm.str("");
	stm << YEL << "It's a draw!" << WHI << "\n\r";
	_sendMessage(stm.str(), ircsock);
	std::cout << YEL << "It's a draw!" << WHI << std::endl;
}



int main()
{
	int ircsock;
    struct sockaddr_in ircHints;

    ircsock = socket(AF_INET, SOCK_STREAM, 0);
    if(ircsock == -1)
    {
        std::cerr << "failed to create socket (ircsock)" << std::endl;
        return 1;
    }

    ircHints.sin_family = AF_INET;
    ircHints.sin_port = htons(4444);
    inet_pton(AF_INET, "127.0.0.1", &(ircHints.sin_addr));
    if(connect(ircsock, (struct sockaddr*)&ircHints, sizeof(ircHints)) == -1) {
        std::cerr << "connect failed\n" << std::endl;
        return 1;
    }
    // connection to irc server
    _sendMessage("PASS 1234\r\n", ircsock);
    _sendMessage("NICK bot\r\n", ircsock);
    _sendMessage("USER bot 0 * bot\r\n", ircsock);

    std::string resp;
    std::string recived;
    ssize_t recivedBytes;
	
    char buff[1024];
	while( (recivedBytes = recv(ircsock, buff, sizeof(buff), 0)) > 0)
    {
        buff[recivedBytes] = '\0';
        std::cout << "recived: " <<  buff;
        recived = buff;
        // if(recived == "PLAY")
		// {
			resp = "PRIVMSG #test :I'm ready to play\n\r";
			_sendMessage(resp, ircsock);
			std::srand((std::time(NULL)));
			playTicTacToe(ircsock);
			int i = GetNumber("Play again? if yes enter 1: \n\r", ircsock);
			if (i != 1)
				{std::cout << "Goodbye!" << std::endl; break;}
		// }
    }
	return 0;
}
