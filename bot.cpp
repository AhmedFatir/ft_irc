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

int ircsock;

void _sendMessage(std::string message, int fd)
{
    if(send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Send failed" << std::endl;
}

int ParsAge(std::string age)
{
	std::string year, month, day, age1;
	age1 = age;
	std::stringstream ss(age);
	int flag = 0;
	while (std::getline(ss, age, '-')){
		flag++;
		for (size_t i = 0; i < age.size(); ++i)
			{if (!isdigit(age[i]) && age[i] != '-') return 0;}
	}
	if (flag != 3) return 0;
	int found = age1.find("-");
	year = age1.substr(0, found);
	age1 = age1.substr(found+1);
	found = age1.find("-");
	month = age1.substr(0, found);
	day = age1.substr(found+1);

	if (std::atoi(year.c_str()) > 2024 || std::atoi(year.c_str()) < 1900)  return 0; // valid year
	if (std::atoi(year.c_str()) == 2024 && std::atoi(month.c_str()) > 1) return 0; // valid month
	if (std::atoi(month.c_str()) > 12 || std::atoi(month.c_str()) < 1 || std::atoi(day.c_str()) > 31 || std::atoi(day.c_str()) < 1 ) return 0; // valid month and day
	if (std::atoi(month.c_str()) == 4 || std::atoi(month.c_str()) == 6 || std::atoi(month.c_str()) == 9 || std::atoi(month.c_str()) == 11) // valid day
		{if (std::atoi(day.c_str()) > 30) return 0;}
	if ((std::atoi(year.c_str()) % 4 == 0 && std::atoi(year.c_str()) % 100 != 0) || std::atoi(year.c_str()) % 400 == 0) // valid leap year
		{if (std::atoi(month.c_str()) == 2 && std::atoi(day.c_str()) > 29) return 0;}
	else
		{if (std::atoi(month.c_str()) == 2 && std::atoi(day.c_str()) > 28) return 0;}
	return 1;
}

void ageCalculator(std::string age, std::string Nickname)
{
	if (!ParsAge(age)){	
    	std::string str = "PRIVMSG " + Nickname + " : Invalid date format\r\n";
		_sendMessage(str, ircsock); return;
	}
		
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
//-----------------------------------------------------------------------------------//
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
	command.erase(command.begin());
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
			if (command == "exit") return -2;
			if(command.empty() || command.size() > 1 || !isdigit(command[0]))
				return -1;
			else break;
		}
	}
	return (std::atoi(command.c_str()));
}
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

void playTicTacToe(int ircsock, std::string UserNick)
{
	std::vector<char> board(9, '-');
	send_privmsg("Welcome to (X | O) Game!", ircsock, UserNick);
	send_privmsg("YOU : X | Computer: O", ircsock, UserNick);
	
	
	char Player = 'X';
	int movesLeft = 9;
	while (movesLeft > 0){
		drawBoard(board, ircsock, UserNick);

		if (Player == 'X'){ // Player's turn
			int move = GetNumber("To Quit Send (exit)/ YOU, enter your move (1-9):", ircsock, UserNick);
			if (move == -2) return;
			if (move == -1 || board[move - 1] != '-'){// Validate the move
				send_privmsg("Invalid move. Try again!", ircsock, UserNick);
				continue;
			}
			board[move - 1] = Player;
		}
		else{ // Computer's turn
			send_privmsg("Computer's turn...", ircsock, UserNick);
			sleep(1);
			SetMove(board, Player, movesLeft);
		}
		if (checkWin(board, Player)){ // Check who wins
			drawBoard(board, ircsock, UserNick);
			if (Player == 'X')
				send_privmsg("YOU win!😁", ircsock, UserNick);
			else
				send_privmsg("Computer wins!🤖", ircsock, UserNick);
			return;
		}
		// Switch players
		if (Player == 'X') Player = 'O';
		else Player = 'X';
		movesLeft--;
	}
	drawBoard(board, ircsock, UserNick);
	send_privmsg("It's a draw!🙂", ircsock, UserNick);
}
bool isPortValid(std::string port)
{
	return (port.find_first_not_of("0123456789") == std::string::npos && std::atoi(port.c_str()) >= 1024 && std::atoi(port.c_str()) < 65535);
}

void signalHandler(int signum)
{
	(void)signum;
	std::string quit = "QUIT\r\n";
	if(send(ircsock, quit.c_str(), quit.size(), 0) == -1)
		std::cerr << "send() faild" << std::endl;
}

//-----------------------------------------------------------------------------------//
std::string getnokta(std::vector<std::string> &vnokat, int size)
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));
	return vnokat[std::rand() % size];
}
std::vector<std::string> getnokat(std::string filename)
{
	std::vector<std::string> vnokat;
	std::string line;
	std::ifstream file(filename);
	if (file.is_open())
	{
		while (std::getline(file, line))
			vnokat.push_back(line);
		file.close();
	}
	return vnokat;
}

void nokta(std::string nick, std::vector<std::string> &vnokat ,int &ircsock)
{
	std::string response = "PRIVMSG " + nick + " : " + getnokta(vnokat, vnokat.size()) + "\r\n";
	_sendMessage(response, ircsock);
}
//-----------------------------------------------------------------------------------//

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
		if (ircsock == -1)
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
	std::string filename = "qoutes";
	std::vector<std::string> vnokat = getnokat(filename);
	while( (recivedBytes = recv(ircsock, buff, sizeof(buff), 0)) > 0)
    {
        buff[recivedBytes] = '\0';
        std::cout << "recived: " <<  buff;
        recived = SplitBuff(buff, UserNick, date);
		std::cout << "splited: " << recived << std::endl;
        if(recived == "PLAY" || recived == "play")
			playTicTacToe(ircsock, UserNick);
        else if(recived == "AGE" || recived == "age")
			ageCalculator(date, UserNick);
		else if (recived == "NOKTA" || recived == "nokta")
			nokta(UserNick,vnokat,ircsock);
	}
	return 0;
}
