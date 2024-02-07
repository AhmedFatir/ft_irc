#include "Bot.hpp"
#include <vector>

void _sendMessage(std::string message, int fd)
{
    if(send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Send failed" << std::endl;
}

std::string age_calculator(std::string age)
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
    return str;
}


std::vector<std::string> split_cmd(std::string& cmd)
{
	std::vector<std::string> vec;
	std::istringstream stm(cmd);
	std::string token;
	while(stm >> token)
	{
		vec.push_back(token);
		token.clear();
	}
	return vec;
}

void send_privmsg(int srvsock,std::string age, std::string nickname)
{
    std::string msg = "PRIVMSG " +nickname + " :your age is: " +age +"\r\n";
    send(srvsock, msg.c_str(), msg.size(),0);
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
        std::cout << "recived: " <<  buff;
        recived = buff;
        std::vector<std::string> splited = split_cmd(recived);
        if(splited[0] == "age")
        {
            resp = age_calculator(splited[1]);
            send_privmsg(ircsock, resp, splited[2]);
        }
        else if (splited[0] == "create")
        {
            resp = "PRIVMSG #" + splited[1] + " : " +splited[2] + " You have just create new channel. " +splited[1] + "\n\r";
            _sendMessage(resp, ircsock);
        }
        // send(ircsock, resp.c_str(), resp.size(), 0);
        resp.clear();
    }
}