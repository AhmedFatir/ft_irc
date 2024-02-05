#include "Client.hpp"

Client::Client()
{
	this->nickname = "";
	this->username = "";
	this->fd = -1;
	this->isOperator= false;
	this->registered = false;
}
Client::Client(std::string nickname, std::string username, int fd) : nickname(nickname), username(username), fd(fd){}
Client::~Client(){}
Client::Client(Client const &src){*this = src;}
Client &Client::operator=(Client const &src){
	if (this != &src){
		this->nickname = src.nickname;
		this->username = src.username;
		this->fd = src.fd;
		this->ChannelsInvite = src.ChannelsInvite;
	}
	return *this;
}

std::string Client::GetNickName(){return this->nickname;}
std::string Client::GetUserName(){return this->username;}
int Client::GetFd(){return this->fd;}
void Client::SetFd(int fd){this->fd = fd;}
void Client::SetNickname(std::string& nickName){this->nickname = nickName;}

std::string Client::getHostname()
{
	std::string hostname = this->GetNickName() + "!" + this->GetUserName();
	return hostname;
}

void Client::AddChannelInvite(std::string name){ChannelsInvite.push_back(name);}

bool Client::GetChannelName(std::string name){
	for (size_t i = 0; i < ChannelsInvite.size(); i++){
		if (ChannelsInvite[i] == name)
			return true;
	}
	return false;
}

