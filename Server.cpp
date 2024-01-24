/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatir <afatir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 10:06:56 by afatir            #+#    #+#             */
/*   Updated: 2024/01/23 18:57:30 by afatir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


Server::Server(){}
Server::Server(int port, std::string password){this->port = port; this->password = password;}
Server::~Server(){}
Server::Server(Server const &src){*this = src;}
Server &Server::operator=(Server const &src){
	if (this != &src){
		this->clients = src.clients;
		this->channels = src.channels;
		this->fds = src.fds;
		this->password = src.password;
		this->port = src.port;
		this->server_fdsocket = src.server_fdsocket;
	}
	return *this;
}
void Server::SetFd(int fd){this->server_fdsocket = fd;}
void Server::SetPort(int port){this->port = port;}
void Server::SetPassword(std::string password){this->password = password;}
int Server::GetFd(){return this->server_fdsocket;}
int Server::GetPort(){return this->port;}
std::string Server::GetPassword(){return this->password;}

void Server::AddClient(Client newClient){this->clients.push_back(newClient);}
void Server::AddChannel(Channel newChannel){this->channels.push_back(newChannel);}
void Server::AddFds(pollfd newFd){this->fds.push_back(newFd);}

void Server::RemoveClient(int fd){
	for (size_t i = 0; i < this->clients.size(); i++){
		if (this->clients[i].GetFd() == fd)
			{this->clients.erase(this->clients.begin() + i); return;}
	}
}
void Server::RemoveChannel(std::string name){
	for (size_t i = 0; i < this->channels.size(); i++){
		if (this->channels[i].GetName() == name)
			{this->channels.erase(this->channels.begin() + i); return;}
	}
}
void Server::RemoveFds(int fd){
	for (size_t i = 0; i < this->fds.size(); i++){
		if (this->fds[i].fd == fd)
			{this->fds.erase(this->fds.begin() + i); return;}
	}
}

//###################################
void Server::init(int port, std::string pass)
{
	this->password = pass;
	this->port = port;
	this->set_sever_socket();
	std::cout << "Waiting to accept a connection...\n";
	while (true)
	{
		if(poll(&fds[0],fds.size(),-1) == -1)
			throw(std::runtime_error("error in poll"));
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents == POLLIN) 
			{
				if (fds[i].fd == server_fdsocket){
					this->accept_new_client();
				}
				else {
					this->accept_new_message(fds[i].fd);
				}
			}
		}
	}
}

void Server::set_sever_socket()
{
	int en = 1;

	add.sin_family = AF_INET;
	add.sin_addr.s_addr = INADDR_ANY;
	add.sin_port = htons(port);
	server_fdsocket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fdsocket == -1)
		throw(std::runtime_error("faild to create socket"));
	if(setsockopt(server_fdsocket, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	if (bind(server_fdsocket, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("faild to bind socket"));
	if (listen(server_fdsocket, SOMAXCONN) == -1)
		throw(std::runtime_error("listen() faild"));
	new_cli.fd = server_fdsocket;
	new_cli.events = POLLIN;
	new_cli.revents = 0;
	fds.push_back(new_cli);
}

void Server::accept_new_client()
{
	socklen_t len = sizeof(cliadd);

	int incofd = accept(server_fdsocket, (sockaddr *)&cliadd, &len);
	if (incofd == -1)
		throw(std::runtime_error("faccept() failed"));
	new_cli.fd = incofd;
	new_cli.events = POLLIN;
	new_cli.revents = 0;
	fds.push_back(new_cli);
}

std::vector<std::string> Server::split_recivedBuffer(std::string &str)
{

	std::vector<std::string> vec;
	std::istringstream stm(str);
	std::string line;
	while(std::getline(stm, line))
		vec.push_back(line);
	return vec;
}

void Server::accept_new_message(int fd)
{
	char buff[1024];
	ssize_t bytes;
	std::vector<std::string> cmd;
	if((bytes = recv(fd, buff, sizeof(buff), 0)) == -1)
		throw(std::runtime_error("recv() faild"));
	if(bytes == 0)
	{
		std::cout << "clinet: " << fd << " disconnected" << std::endl;
		RemoveClient(fd);
		RemoveFds(fd);
		close(fd);
	}
	else
	{
		buff[bytes] = '\0';
		std::string recived = buff;
		std::cout << "recived: " << recived;
		cmd = split_recivedBuffer(recived);
		for(size_t i = 0; i < cmd.size(); i++)
			this->parse_exec_cmd(cmd[i], fd);
	}
}


// bool Server::is_channlExist(std::string& channel_name)
// {
// 	for (size_t i = 0; i < this->channels.size(); i++){
// 		if (this->channels[i].GetName() == channel_name)
// 			return true;
// 	}
// 	return false;
// }
// void Server::join_channel(std::vector<std::string>& cmd, int fd)
// {
// 	(void)fd;
// 	if(this->is_channlExist(cmd[1]))
// 		std::cout << "exist" << std::endl;
// 	else
// 	{
// 		Channel ch;
// 		ch.SetName = cmd[1];
// 		this->AddChannel()
// 		std::cout << "does not exist" << std::endl;
// 	}

// }

std::vector<std::string> Server::split_cmd(std::string& cmd)
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

void Server::parse_exec_cmd(std::string &cmd, int fd)
{
	std::vector<std::string> splited_cmd = split_cmd(cmd);
    if(splited_cmd[0] == "PASS")
        client_authen(fd, splited_cmd[1]);
	else if (splited_cmd[0] == "NICK")
		set_nickname(splited_cmd[1],fd);
	// else if (splited_cmd[0] == "USER")
	// 	set_username(splited_cmd, fd);

}

// Client*	Server::isCliExist(int fd)
// {
// 	for (size_t i = 0; i < this->clients.size(); i++)
// 	{
// 		if (this->clients[i].GetFd() == fd)
// 			return &(this->clients[i]);	
// 	}
// 	return NULL;
// }


// void	Server::set_username(std::vector<std::string>& cmd, int fd)
// {

// }


//    Numeric Replies:

//    ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
//    ERR_NICKNAMEINUSE               ERR_NICKCOLLISION
//    ERR_UNAVAILRESOURCE             ERR_RESTRICTED
void Server::set_nickname(std::string& nickname, int fd)
{
	std::cout << "		nick name to set: |" << nickname << "|\n"; 
	if(nickname == ":" || nickname.empty())
	{
		std::string resp = ":localhost 431 user :No nickname given\r\n";
		if(send(fd,resp.c_str(), resp.size(), 0) == -1)
			throw(std::runtime_error("send() faild"));
	}
	// else if (is_nicknameInUSe(nickname))
	// {
		
	// }
	else
	{
		this->clients[fd].SetNickname(nickname);
		// nickname
	}
	// this->clients[fd].SetNickname(nickname);
	// for (size_t i = 0; i < this->clients.size(); i++)
	// {
	// 	if (this->clients[i].GetFd() == fd)
	// 	{
	// 		this->clients[i].SetNickname(nickname);
	// 		break;
	// 	}
	// }
	std::cout << "		clinet nickname: |x" << this->clients[fd].GetNickName() << "x|" << std::endl;
}

void Server::client_authen(int fd, std::string& pass)
{
	std::vector<std::string> cmd;
	Client cli;
	if(pass == password)
	{
		cli.SetFd(fd);
		clients.push_back(cli);
		std::string welcome = ":localhost 001 user :Welcome to the IRC server!\r\n";
		send(fd, welcome.c_str(), welcome.size(), 0);
	}
	else
	{
		std::string resp = ":localhost 464 user :Password incorrect\r\n";
		send(fd, resp.c_str(), resp.size(),0);
		close(fd);
		RemoveFds(fd);
		RemoveClient(fd);
	}

}
