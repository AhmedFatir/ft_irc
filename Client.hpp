#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include "Channel.hpp"


class Client
{
private:
	std::string nickname;
	std::string username;
	bool isOperator;
	int fd;
	bool registered;
	std::vector<std::string> ChannelsInvite;
public:
	Client();
	Client(std::string nickname, std::string username, int fd);
	~Client();
	Client(Client const &src);
	Client &operator=(Client const &src);

	std::string GetNickName();
	std::string GetUserName();
	void SetFd(int fd);
	int GetFd();
	std::string getHostname();
	//###############
	void SetNickname(std::string& nickName);
	void SetUsername(std::string& username)
	{
		this->username = username;
	}
	void AddChannelInvite(std::string name);
	bool GetChannelName(std::string name);
};

#endif