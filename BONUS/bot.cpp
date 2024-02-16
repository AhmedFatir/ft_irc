
#include "Bot.hpp"

//-----------------------------------------------------------------------------------//
bool isPortValid(std::string port)
{
	return (port.find_first_not_of("0123456789") == std::string::npos && std::atoi(port.c_str()) >= 1024 && std::atoi(port.c_str()) < 65535);
}

void signalHandler(int signum)
{
	(void)signum;
	std::string quit = "QUIT\r\n";
	if(send(ircsock, quit.c_str(), quit.size(), 0) == -1)
		std::cerr << "send faild" << std::endl;
}

//-----------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------//

int main(int ac, char **av)
{
	Bot bot;
	if (ac != 4)
	{std::cerr << "Usage: " << av[0] << " <address> <port> <password> <file>" << std::endl; return 1;}
	if (!isPortValid(av[2]) || !*av[3] || std::strlen(av[3]) > 20)
		{std::cerr << "Invalid port! / password!" << std::endl; return 1;}
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	std::string address = av[1];
	if(address == "localhost" || address == "LOCALHOST")
		address = "127.0.0.1";
	struct sockaddr_in ircHints;
	ircsock = socket(AF_INET, SOCK_STREAM, 0);
	if (ircsock == -1)
		{std::cerr << "failed to create socket (ircsock)" << std::endl; return 1;}
	
    ircHints.sin_family = AF_INET;
    ircHints.sin_port = htons(std::atoi(av[2]));
    if(!inet_pton(AF_INET, address.c_str(), &(ircHints.sin_addr)))
		return 0;
	
    if(connect(ircsock, (struct sockaddr*)&ircHints, sizeof(ircHints)) == -1)
		{std::cerr << "connect failed" << std::endl; return 1;}
	
    // connection to irc server
    _sendMessage("PASS " + std::string(av[3]) + "\r\n", ircsock);
    _sendMessage("NICK bot\r\n", ircsock);
    _sendMessage("USER bot 0 * bot\r\n", ircsock);
	bot.init();
	return 0;
}
