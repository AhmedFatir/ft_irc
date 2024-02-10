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

void send_privmsg(std::string message, int srvsock, std::string UserNick)
{
	std::cout << message << std::endl;
    std::string msg = "PRIVMSG " + UserNick + " :" + message + "\r\n";
    if (send(srvsock, msg.c_str(), msg.size(),0) == -1)
		std::cerr << "Send failed" << std::endl;
}

void _sendMessage(std::string message, int fd)
{
    if(send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Send failed" << std::endl;
}

void drawBoard(const std::vector<char>& board, int ircsock, std::string UserNick)
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
		{send_privmsg(line, ircsock, UserNick); line.clear();}
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

std::string SplitBuff(std::string buff)
{
	std::string command;
	size_t found = buff.find("bot");
	if (found != std::string::npos)
		command = buff.substr(found+4);
	else return "";
	found = command.find("\r\n");
	if (found != std::string::npos)
		command = command.substr(0, found);
	return command;
}
std::string SplitBuff(std::string buff, std::string &UserNick)
{
	std::istringstream stm(buff);
	std::string token;
	stm >> UserNick;
	stm >> token;
	return token;
}
// Function to play the Tic-Tac-Toe game against the computer
int GetNumber(std::string prompt, int ircsock, std::string UserNick)
{
	ssize_t recivedBytes;
    char buff[1024];

  	std::string	command;
	while (1){
		send_privmsg(prompt, ircsock, UserNick);
		if ((recivedBytes = recv(ircsock, buff, sizeof(buff), 0)) > 0){
			command = SplitBuff(buff);
			if(command.empty() || command.size() > 1 || !isdigit(command[0]))
				return -1;
			else break;
		}
	}
	return (std::atoi(command.c_str()));
}

void playTicTacToe(int ircsock, std::string UserNick)
{
	std::vector<char> board(9, '-');
	send_privmsg("Welcome to (X | O) Game!", ircsock, UserNick);
	send_privmsg("YOU : X | Computer: O", ircsock, UserNick);
	
	
	char Player = 'X';
	int movesLeft = 9;
	while (movesLeft > 0){
		std::srand((std::time(NULL)));
		drawBoard(board, ircsock, UserNick);

		if (Player == 'X'){ // Player's turn
			int move = GetNumber("YOU, enter your move (1-9):", ircsock, UserNick);
			if (move == -1 || board[move - 1] != '-'){// Validate the move
				send_privmsg("Invalid move. Try again!", ircsock, UserNick);
				continue;
			}
			board[move - 1] = Player;
		}
		else{ // Computer's turn
			send_privmsg("Computer's turn...", ircsock, UserNick);
			sleep(1);
			int Move;
			while (1){
				Move = std::rand() % 9;
				if (board[Move] == '-')
					{board[Move] = Player; break;}
			}
		}
		if (checkWin(board, Player)){ // Check who wins
			drawBoard(board, ircsock, UserNick);
			if (Player == 'X')
				send_privmsg("YOU win!", ircsock, UserNick);
			else
				send_privmsg("Computer wins!", ircsock, UserNick);
			return;
		}
		// Switch players
		if (Player == 'X') Player = 'O';
		else Player = 'X';
		movesLeft--;
	}
	drawBoard(board, ircsock, UserNick);
	send_privmsg("It's a draw!", ircsock, UserNick);
}
bool isPortValid(std::string port)
{
	return (port.find_first_not_of("0123456789") == std::string::npos && std::atoi(port.c_str()) >= 1024 && std::atoi(port.c_str()) < 65535);
}

int main(int ac, char **av)
{
	if (ac != 3)
	{std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl; return 1;}
	if (!isPortValid(av[1]))
		{std::cerr << "Invalid port!" << std::endl; return 1;}
	int ircsock;
    struct sockaddr_in ircHints;

    ircsock = socket(AF_INET, SOCK_STREAM, 0);
    if(ircsock == -1)
    	{std::cerr << "failed to create socket (ircsock)" << std::endl; return 1;}

    ircHints.sin_family = AF_INET;
    ircHints.sin_port = htons(std::atoi(av[1]));
    inet_pton(AF_INET, "127.0.0.1", &(ircHints.sin_addr));
    if(connect(ircsock, (struct sockaddr*)&ircHints, sizeof(ircHints)) == -1)
		{std::cerr << "connect failed\n" << std::endl; return 1;}
    // connection to irc server
    _sendMessage("PASS " + std::string(av[2]) + "\r\n", ircsock);
    _sendMessage("NICK bot\r\n", ircsock);
    _sendMessage("USER bot 0 * bot\r\n", ircsock);

    std::string resp, recived, UserNick;
    ssize_t recivedBytes;

    char buff[1024];
	while( (recivedBytes = recv(ircsock, buff, sizeof(buff), 0)) > 0)
    {
        buff[recivedBytes] = '\0';
        std::cout << "recived: " <<  buff;
        recived = SplitBuff(buff, UserNick);
        if(recived == "PLAY")
			playTicTacToe(ircsock, UserNick);
        // if(recived == "AGE")
		// {
		// 	return 0;
		// }
    }
	return 0;
}
