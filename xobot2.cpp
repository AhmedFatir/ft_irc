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
	{
		_sendMessage("PRIVMSG aa :"+line +"\r\n", ircsock);
		line.clear();
	}
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
		_sendMessage("PRIVMSG aa : " + prompt, ircsock);
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

void playTicTacToe(int ircsock)
{
	std::vector<char> board(9, '-');
	std::stringstream stm;
	stm << "PRIVMSG aa : Welcome to (X | O) Game!" << std::endl;
	stm << "YOU : "  << "X" << " | Computer: " <<  "O" << "\r\n";
	_sendMessage(stm.str(), ircsock);
	std::cout << stm.str() << std::endl;
	
	
	char currentPlayer = 'X';
	int movesLeft = 9;
	while (movesLeft > 0){
		drawBoard(board, ircsock);

		if (currentPlayer == 'X'){ // Player's turn
			int move = GetNumber("YOU, enter your move (1-9): \r\n", ircsock);
			if (move == -1 || board[move - 1] != '-'){// Validate the move
				stm.str("");
				stm << "Invalid move. Try again!" << "\r\n";
				_sendMessage(stm.str(), ircsock);
				std::cout << "Invalid move. Try again!" << std::endl;
				continue;
			}
			board[move - 1] = currentPlayer;
		}
		else{ // Computer's turn
			stm.str("");
			stm << "Computer's turn..." << "\r\n";
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
				stm << "YOU win!" << "\r\n";
				_sendMessage(stm.str(), ircsock);
				std::cout  << "YOU win!"  << std::endl;
			}
			else{
				stm.str("");
				stm  << "Computer wins!" << "\r\n";
				_sendMessage(stm.str(), ircsock);
				std::cout  << "Computer wins!" << std::endl;
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
	stm  << "It's a draw!" << "\r\n";
	_sendMessage(stm.str(), ircsock);
	std::cout << "It's a draw!" << std::endl;
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
    ircHints.sin_port = htons(9009);
    inet_pton(AF_INET, "127.0.0.1", &(ircHints.sin_addr));
    if(connect(ircsock, (struct sockaddr*)&ircHints, sizeof(ircHints)) == -1) {
        std::cerr << "connect failed\n" << std::endl;
        return 1;
    }
    // connection to irc server
    _sendMessage("PASS 123\r\n", ircsock);
    _sendMessage("NICK bot\r\n", ircsock);
    _sendMessage("USER bot 0 * bot\r\n", ircsock);

    std::string resp;
    std::string recived;
    ssize_t recivedBytes;
	
    char buff[1024];
	while( (recivedBytes = recv(ircsock, buff, sizeof(buff), 0)) > 0)
    {
        buff[recivedBytes] = '\0';
        std::cout << "recived: " <<  buff << "|\n";
        recived = buff;
        if(recived == "PLAY\r\n")
		{
			resp = "PRIVMSG aa :I'm ready to play\r\n";
			_sendMessage(resp, ircsock);
			std::srand((std::time(NULL)));
			playTicTacToe(ircsock);
			int i = GetNumber("Play again? if yes enter 1: \r\n", ircsock);
			if (i != 1)
			{
				std::cout << "Goodbye!" << std::endl; break;
				_sendMessage("QUIT\r\n", ircsock);
			}
		}
    }
	return 0;
}
