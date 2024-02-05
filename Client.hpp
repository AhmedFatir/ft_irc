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
public:
	std::vector<std::string> ChannelsInvite;
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
	{this->username = username;}
	void AddChannelInvite(std::string &chname);
	void RmChannelInvite(std::string &chname);
	bool GetInviteChannel(std::string &ChName);
};

#endif