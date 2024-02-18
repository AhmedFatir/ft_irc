#include "../INC/Bot.hpp"

int ircsock;

bool isPortValid(std::string port){
	return (port.find_first_not_of("0123456789") == std::string::npos && \
	std::atoi(port.c_str()) >= 1024 && std::atoi(port.c_str()) <= 65535);
}

void _sendMessage(std::string message, int fd){
    if(send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Send failed" << std::endl;
}

void SignalHandler(int signum){
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	_sendMessage("QUIT\r\n", ircsock);
}

bool isValidNickname(std::string nickname)
{
	if(!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':' || std::isdigit(nickname[0])))
		return false;
	for(size_t i = 1; i < nickname.size(); i++)
	{
		if(!std::isalnum(nickname[i]) && nickname[i] != '{' && nickname[i] != '}' && \
			nickname[i] != '[' && nickname[i] != ']' && nickname[i] != '|' && nickname[i] != '\\')
			return false;
	}
	return true;
}

int main(int ac, char **av)
{
	Bot bot;
	if (ac != 6)
	{std::cerr << "Usage: " << av[0] << " <address> <port> <password> <botnickname> <file>" << std::endl; return 1;}
	if (!isPortValid(av[2]) || !*av[3] || std::strlen(av[3]) > 20)
		{std::cerr << "Invalid port! / password!" << std::endl; return 1;}
	if(!isValidNickname(av[4]))
		{std::cerr << "Invalid nickname" << std::endl; return 1;}
	signal(SIGINT, SignalHandler);
	signal(SIGQUIT, SignalHandler);
	std::string address = av[1];
	if(address == "localhost" || address == "LOCALHOST")
		address = "127.0.0.1";
	std::string filename = av[5];
	if (!bot.getnokat(filename))
		{std::cerr << "Failed to get nokat" << std::endl; return 1;}
	struct sockaddr_in ircHints;
	ircsock = socket(AF_INET, SOCK_STREAM, 0);
	if (ircsock == -1)
		{std::cerr << "failed to create socket (ircsock)" << std::endl; return 1;}

    ircHints.sin_family = AF_INET;
    ircHints.sin_port = htons(std::atoi(av[2]));
	ircHints.sin_addr.s_addr = inet_addr(address.c_str());
	
    if(connect(ircsock, (struct sockaddr*)&ircHints, sizeof(ircHints)) == -1)
		{std::cerr << "connect failed" << std::endl; return 1;}
	
    // connection to irc server
    _sendMessage("PASS " + std::string(av[3]) + "\r\n", ircsock);
    _sendMessage("NICK " + std::string(av[4]) + "\r\n", ircsock);
    _sendMessage("USER " + std::string(av[4]) + " 0 * bot\r\n", ircsock);
	std::cout << GRE << "BOT Is Connected!" << WHI << std::endl;
	bot.init(ircsock);
	std::cout << RED << "BOT Is Disconnected!" << WHI << std::endl;
	return 0;
}