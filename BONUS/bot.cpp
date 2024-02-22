#include "../INC/Bot.hpp"

Bot::Bot(){this->Loggedin = false;}
Bot::~Bot(){}
Bot::Bot(const Bot& other){*this = other;}
Bot& Bot::operator=(const Bot& other){
	if (this != &other){
		this->players = other.players;
		this->vnokat = other.vnokat;
		this->Loggedin = other.Loggedin;
		this->botnick = other.botnick;
	}
	return *this;
}
void	Bot::setNick(std::string nick)
{
	botnick = nick;
}

//---------------------------------------------------send methods
void Bot::_sendMessage(std::string message, int fd)
{
	if(send(fd, message.c_str(), message.size(), 0) == -1)
		std::cerr << "Send failed" << std::endl;
}

void Bot::send_privmsg(std::string message, std::string UserNick, int ircsock)
{
	std::string msg = "PRIVMSG " + UserNick + " :" + message + "\r\n";
	if (send(ircsock, msg.c_str(), msg.size(),0) == -1)
		std::cerr << "Send failed" << std::endl;
}
//---------------------------------------------------nokat methods
int Bot::getnokat(std::string filename)
{
	std::string line;
	std::ifstream file(filename);
	if (!file.is_open())
		{std::cerr << "Failed to open file" << std::endl; return 0;}
	while (std::getline(file, line))
		vnokat.push_back(line);
	file.close();
	return 1;
}

std::string Bot::getnokta(std::vector<std::string> &vnokat, int size)
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));
	return vnokat[std::rand() % size];
}
//---------------------------------------------------age methods
int Bot::ParsAge(std::string age)
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

void Bot::ageCalculator(std::string age, std::string Nickname,int ircsock)
{
	if (!ParsAge(age))
		{send_privmsg("Invalid date format(<age> <year-month-day>)", Nickname, ircsock);return;}
	int year, month, day;
	year = std::atoi(age.substr(0, 4).c_str());
	month = std::atoi(age.substr(5, 2).c_str());
	day = std::atoi(age.substr(8, 2).c_str());

	std::tm birth_date;
	memset(&birth_date, 0, sizeof(birth_date));
	birth_date.tm_year = year - 1900;
	birth_date.tm_mon = month - 1;
	birth_date.tm_mday = day;

	std::time_t birth_time = mktime(&birth_date);
	std::time_t current_time;
	std::time(&current_time);

	double seconds = difftime(current_time, birth_time);
	int years = static_cast<int>(seconds / (365.25 * 24 * 60 * 60));
	int months = static_cast<int>((seconds - years * 365.25 * 24 * 60 * 60) / (30.44 * 24 * 60 * 60));
	int days = static_cast<int>((seconds - years * 365.25 * 24 * 60 * 60 - months * 30.44 * 24 * 60 * 60) / (24 * 60 * 60));
	std::stringstream ss;
	ss << "Your Age is: " << years << " years, " << months << " months, " << days << " days old";
	send_privmsg(ss.str(), Nickname, ircsock);
}
//---------------------------------------------------parse methods
void Bot::getCommand(std::string recived, std::string& nick , std::string &command)
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
std::string Bot::SplitBuff(std::string buff, std::string &date)
{
	std::istringstream stm(buff);
	std::string token;
	stm >> token;
	stm >> date;
	return token;
}
//---------------------------------------------------game methods
void Bot::drawBoard(const std::vector<char>& board, std::string UserNick, int ircsock)
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
		{send_privmsg(line, UserNick, ircsock); line.clear();}
}

bool Bot::checkWin(const std::vector<char>& board, char player)
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

int Bot::PotentialAction(const std::vector<char>& board, char player)
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

void Bot::SetMove(std::vector<char>& board, char Player, int move)
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

void Bot::PlayGame(std::string command, std::string nickname, int ircsock)
{
	Player *plyr = GetPlayer(nickname);
	if(command == "play" && !plyr){// if the player is not in the list (first time)
		Player player;
		std::vector<char> board(9, '-');
		player.board = board;
		player.setIsPlaying(true);
		player.setNickname(nickname);
		players.push_back(player);
		send_privmsg("Welcome to (X | O) Game!",  nickname, ircsock);
		send_privmsg("YOU : X | Computer: O", nickname, ircsock);
		drawBoard(player.board, nickname, ircsock);
	}
	else if (command == "play" && plyr && !plyr->getIsPlaying()){ // if the player is in the list but not playing
		send_privmsg("Welcome to (X | O) Game!",  nickname, ircsock);
		send_privmsg("YOU : X | Computer: O", nickname, ircsock);
		std::vector<char> board(9, '-');
		plyr->restMoves();
		plyr->board = board;
		drawBoard(plyr->board, nickname, ircsock);
		plyr->setIsPlaying(true);
	}
	else if (plyr && plyr->getIsPlaying())// if the player is in the list and playing
	{
		if(command.empty() || command.size() > 1 || !isdigit(command[0]) || std::atoi(command.c_str()) < 1 || plyr->getChatAtPos(std::atoi(command.c_str()) - 1) != '-'){
			send_privmsg("Invalid move. Try again!", nickname, ircsock);
			send_privmsg("To Quit Send (exit)/ YOU, enter your move (1-9):", nickname, ircsock);
			drawBoard(plyr->board, nickname, ircsock);
			return;
		}
		int move = std::atoi(command.c_str());
		plyr->setChatAtPos(move - 1, 'X');
		drawBoard(plyr->board, nickname, ircsock);
		plyr->dicMoves();
		if (checkWin(plyr->board, 'X') || !plyr->getMoves()){
			if(!plyr->getMoves())
				send_privmsg("It's a draw!🙂", nickname, ircsock);
			else
				send_privmsg("YOU win!😁", nickname, ircsock);
			plyr->setIsPlaying(false);
			plyr->board.clear();
			return;
		}
		//computer's turn
		send_privmsg("Computer's turn ...", nickname, ircsock);
		sleep(1);
		SetMove(plyr->board, 'O', plyr->getMoves());
		drawBoard(plyr->board, nickname, ircsock);
		if (checkWin(plyr->board, 'O') || !plyr->getMoves()){// Check who wins
			if(!plyr->getMoves())
				send_privmsg("It's a draw!🙂", nickname, ircsock);
			else
				send_privmsg("Computer wins!🤖", nickname, ircsock);
			plyr->setIsPlaying(false);
			plyr->board.clear();
			return;
		}
		plyr->dicMoves();
	}
	else
		send_privmsg("Invalid command. Try again!", nickname, ircsock);
}
//---------------------------------------------------init methods
Player *Bot::GetPlayer(std::string nickname)
{	
	for (size_t i = 0; i < this->players.size(); i++){
		if (this->players[i].getNickname() == nickname)
			return &this->players[i];
	}
	return NULL;

}

void Bot::init(int ircsock)
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
		size_t pos = recived.find_first_of("\n\r");
		if(pos != std::string::npos)
			recived = recived.substr(0, pos);
		if(recived == ": 001 " + botnick + " : Welcome to the IRC server!" && !Loggedin)
			Loggedin = true;
		else if (!Loggedin){
			std::cout << recived << std::endl;
			return;
		}
		else if(recived.find("PRIVMSG") != std::string::npos && Loggedin)
		{
			getCommand(recived, nickname, command);
			Player *plyr = GetPlayer(nickname);
			if(command.find("age") != std::string::npos){
				if (plyr && plyr->getIsPlaying()){
					plyr->setIsPlaying(false);
					plyr->board.clear();
					send_privmsg("Game Closed!", nickname, ircsock);
				}
				std::string date;
				SplitBuff(command, date);
				ageCalculator(date, nickname, ircsock);
				continue;
			}
			else if(command.find("nokta") != std::string::npos){
				if (plyr && plyr->getIsPlaying()){
					plyr->setIsPlaying(false);
					plyr->board.clear();
					send_privmsg("Game Closed!", nickname, ircsock);
				}
				send_privmsg(getnokta(vnokat, vnokat.size()), nickname, ircsock);
				continue;
			}
			else if(command == "exit" && plyr && plyr->getIsPlaying()){
				plyr->setIsPlaying(false);
				plyr->board.clear();
				send_privmsg("Game Closed!", nickname, ircsock);
				continue;
			}
			else
				PlayGame(command, nickname, ircsock);
		}
	}
}
