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
#include "Server.hpp"
int ircsock;

void _sendMessage(std::string message, int fd)
{
    if(send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Send failed" << std::endl;
}
//----------
std::string getpickline(const std::string pickuplines[], int size)
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));
	int index = std::rand() % size;
	return pickuplines[index];
}
void Server::pickuplinesbot(std::string &cmd, int &fd)
{
	std::vector<std::string> scmd = split_cmd(cmd);
	if (scmd.size() != 1)
	{
		senderror(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n");
		return;
	}
	const std::string pickuplines[] = {
		"When I look in your eyes, I see a very kind soul.",
		"Do you happen to have a Band-Aid? ‘Cause I scraped my knees falling for you.",
		"I never believed in love at first sight, but that was before I saw you.",
		"f being sexy was a crime, you’d be guilty as charged.",
		"Do you have a map? I just got lost in your eyes.",
		"I would never play hide and seek with you because someone like you is impossible to find.",
		"Want to go outside and get some fresh air with me? You just took my breath away.",
		"Is your name Google? Because you have everything I’m searching for.",
		"I must be a snowflake because I’ve fallen for you.",
		"I’m not a photographer, but I can definitely picture us together.",
		"I’m not a genie, but I can make your dreams come true.",
		"You know, your smile has been lighting up the room all night and I just had to come and say hello.",
		"I can’t tell if that was an earthquake, or if you just seriously rocked my world.",
	};
	std::string resp = "💓 " + getpickline(pickuplines, sizeof(pickuplines) / sizeof(pickuplines[0])) + " 💓";
	std::string botmsg = "PRIVMSG " + GetClient(fd)->GetNickName() + " : " + resp + "\r\n";
	_sendMessage(botmsg, fd);
}
//----------

void ageCalculator(std::string age, std::string Nickname)
{
    int year, month, day;
    year = std::atoi(age.substr(0, 4).c_str());
    month = std::atoi(age.substr(5, 2).c_str());
    day = std::atoi(age.substr(8, 2).c_str());

    std::cout << "year: " << year << "month: " << month << "day: " << day << std::endl;
    std::tm date;
    memset(&date, 0, sizeof(date));
    date.tm_year = year - 1900;
    date.tm_mon = month - 1;
    date.tm_mday = day;

    std::time_t age_in_sec = mktime(&date);
    std::time_t current_time;
    std::time(&current_time);
    int age_int = (current_time - age_in_sec) / (365.25 * 24 * 60 * 60);
    std::stringstream ss;
    ss << age_int;
    std::string str = ss.str();
    str = "PRIVMSG " + Nickname + " : Your age is : " + str + " year(s) old\r\n";
	_sendMessage(str, ircsock); 
}

void send_privmsg(std::string message, int srvsock, std::string UserNick)
{
	std::cout << message << std::endl;
    std::string msg = "PRIVMSG " + UserNick + " :" + message + "\r\n";
    if (send(srvsock, msg.c_str(), msg.size(),0) == -1)
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
std::string SplitBuff(std::string buff, std::string &UserNick, std::string &date)
{
	std::istringstream stm(buff);
	std::string token;
	stm >> UserNick;
	stm >> token;
	stm >> date;
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

void signalHandler(int signum)
{
	(void)signum;
	std::string quit = "QUIT\r\n";
	if(send(ircsock, quit.c_str(), quit.size(), 0))
		std::cerr << "send() faild" << std::endl;
}

int main(int ac, char **av)
{
	if (ac != 3)
	{std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl; return 1;}
	if (!isPortValid(av[1]))
		{std::cerr << "Invalid port!" << std::endl; return 1;}
	// int ircsock;
    struct sockaddr_in ircHints;
	signal(SIGINT, signalHandler);
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

    std::string recived, UserNick, date;
    ssize_t recivedBytes;

    char buff[1024];
	while( (recivedBytes = recv(ircsock, buff, sizeof(buff), 0)) > 0)
    {
        buff[recivedBytes] = '\0';
        std::cout << "recived: " <<  buff;
        recived = SplitBuff(buff, UserNick, date);
		std::cout << "splited: " << recived << std::endl;
        if(recived == "PLAY")
			playTicTacToe(ircsock, UserNick);
        else if(recived == "AGE")
			ageCalculator(date, UserNick);
    }
	return 0;
}
