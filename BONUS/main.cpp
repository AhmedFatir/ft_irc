#include "../INC/Bot.hpp"

int ircsock;

int parse(std::vector<std::string> &values){

	std::vector<std::pair<std::string, std::string> > envs;
	std::string content,line, key, value;
	std::fstream file("BONUS/.env");
	if (!file.is_open())
		{std::cerr << RED << "Failed to open .env file" << WHI << std::endl; return 1;}
	while (std::getline(file, line))
		content += line + '\n';
	file.close();
	std::stringstream ss(content);
	while (std::getline(ss, line, '\n')){
		std::stringstream ss2(line);
		std::getline(ss2, key, '=');
		std::getline(ss2, value, '=');
		envs.push_back(std::make_pair(key, value));
	}
	if (envs.size() != 5)
		{std::cerr << RED << "Invalid number of ENV variables" << WHI << std::endl; return 1;}
	values.push_back("ADDRESS");
	values.push_back("PORT");
	values.push_back("PASSWORD");
	values.push_back("NICKNAME");
	values.push_back("FILE");
	for (size_t i = 0; i < envs.size(); i++){
		if (envs[i].first != values[i])
			{std::cerr << RED << "Invalid ENV variable" << WHI << std::endl; return 1;}
		if ( i != 2 && (std::isspace(envs[i].second[0]) || std::isspace(envs[i].second[envs[i].second.size() - 1])))
			{std::cerr << RED << "Invalid ENV values"<< WHI << std::endl; return 1;}
	}
	for (size_t i = 0; i < envs.size(); i++)
		values[i] = envs[i].second;
	return 0;
}

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

int main()
{
	Bot bot;
	std::cout << "The bot takes its arguments from the .env file. You may modify the values if necessary." << std::endl;
	std::vector<std::string> av;
	if (parse(av))
		return 1;
	if (!isPortValid(av[1]) || av[2].empty() || av[2].size() > 20)
		{std::cerr << RED << "Invalid port! / password!" << WHI << std::endl; return 1;}
	signal(SIGINT, SignalHandler);
	signal(SIGQUIT, SignalHandler);
	std::string address = av[0];
	if(address == "localhost" || address == "LOCALHOST")
		address = "127.0.0.1";
	std::string filename = av[4];
	if (!bot.getnokat(filename))
		{std::cerr << RED << "Failed to get nokat" << WHI << std::endl; return 1;}
	struct sockaddr_in ircHints;
	ircsock = socket(AF_INET, SOCK_STREAM, 0);
	if (ircsock == -1)
		{std::cerr << "failed to create socket (ircsock)" << std::endl; return 1;}

    ircHints.sin_family = AF_INET;
    ircHints.sin_port = htons(std::atoi(av[1].c_str()));
	ircHints.sin_addr.s_addr = inet_addr(address.c_str());
	
    if(connect(ircsock, (struct sockaddr*)&ircHints, sizeof(ircHints)) == -1)
		{std::cerr << RED << "connect() failed" << WHI << std::endl; return 1;}
	
    // connection to irc server
    _sendMessage("PASS " + std::string(av[2]) + "\r\n", ircsock);
    _sendMessage("NICK " + std::string(av[3]) + "\r\n", ircsock);
    _sendMessage("USER " + std::string(av[3]) + " 0 * bot\r\n", ircsock);
	bot.setNick(std::string(av[3]));
	std::cout << GRE << "BOT Is Connected!" << WHI << std::endl;
	bot.init(ircsock);
	std::cout << RED << "BOT Is Disconnected!" << WHI << std::endl;
	return 0;
}