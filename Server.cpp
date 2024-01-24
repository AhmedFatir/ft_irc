/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatir <afatir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 10:06:56 by afatir            #+#    #+#             */
/*   Updated: 2024/01/24 16:10:32 by afatir           ###   ########.fr       */
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
Client *Server::GetClient(int fd){
	for (size_t i = 0; i < this->clients.size(); i++){
		if (this->clients[i].GetFd() == fd)
			return &this->clients[i];
	}
	return NULL;
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
					this->accept_new_message(fds[i].fd, i);
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
	std::string welcome = ":localhost 001 user :Welcome to the IRC server!\r\n";
	send(incofd, welcome.c_str(), welcome.size(), 0);
}

std::vector<std::string> Server::split_cmd(std::string &str)
{
	std::istringstream stm(str);
	std::string token;
	std::vector<std::string> vec;
	while(stm >> token)
	{
		vec.push_back(token);
		token.clear();
	}
	return vec;
}

void Server::accept_new_message(int fd, size_t pos)
{
	char buff[1024];
	ssize_t bytes;
	std::vector<std::string> cmd;
	if((bytes = recv(fd, buff, sizeof(buff), 0)) == -1)
		throw(std::runtime_error("recv() faild"));
	if(bytes == 0)
	{
		std::cout << ": >> " << bytes << std::endl;
		std::cout << "clinet: " << fd << " disconnected" << std::endl;
		close(fd);
		fds.erase(fds.begin() + pos);
	}
	else
	{
		buff[bytes] = '\0';
		std::string recived = buff;
		std::cout << "data recived: " << recived;
		cmd = split_cmd(recived);
		// this->parse_exec_cmd(cmd, fd, pos);
	}
}

// void Server::parse_exec_cmd(std::vector<std::string>& cmd, int fd)
// {
//     if(cmd[0] == "PASS")
//         client_authen(fd, cmd[1]);

// }


void Server::client_authen(int fd, std::string& pass, std::vector<struct pollfd> &fds)
{
	std::vector<std::string> cmd;
	Client cli;
	if(pass == password)
	{
		cli.SetFd(fd);
		clients.push_back(cli);
	}
	else
	{
		std::string resp = ":localhost 464 user :Password incorrect\r\n";
		send(fd, resp.c_str(), resp.size(),0);
		close(fd);
		RemoveFds(fd);
		RemoveClient(fd);
		(void)fds;
	}
}
//######################################################################################
// functions to join a channel
void printVector(std::vector<std::string> token)
{
	for (size_t i = 0; i < token.size(); i++)
		std::cout << "|" << token[i] << "|";
	std::cout << std::endl;	
}
void SplitCmdJoin(std::string cmd, std::vector<std::string> &tmp)
{
	std::string str;
	for (size_t i = 0; i < cmd.size(); i++){
		if (cmd[i] == ' ' || cmd[i] == ',')
			{tmp.push_back(str); str.clear();}
		else str += cmd[i];
	}
	tmp.push_back(str);
	tmp.erase(tmp.begin());
}

void SplitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd)
{
	std::vector<std::string> tmp;
	SplitCmdJoin(cmd, tmp);
	for (size_t i = 0; i < tmp.size(); i++)
	{
		std::string str, pass;
		if (tmp[i][0] == '#'){
			str = tmp[i];
			for (size_t j = i + 1; j < tmp.size(); j++){
				if (tmp[j][0] != '#' && tmp[j][0] != '&')
					{pass = tmp[j];tmp.erase(tmp.begin() + j);break;}
			}
			token.push_back(std::make_pair(str, pass));
		}
		else if (tmp[i][0] == '&'){
			str = tmp[i];
			for (size_t j = i + 1; j < tmp.size(); j++){
				if (tmp[j][0] != '#' && tmp[j][0] != '&')
					{pass = tmp[j]; tmp.erase(tmp.begin() + j); break;}
			}
			token.push_back(std::make_pair(str, pass));
		}
		else token.push_back(std::make_pair(tmp[i], ""));
	}
}

void senderror(int code,std::string clientname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << msg;
	std::string resp = ss.str();
	send(fd, resp.c_str(), resp.size(),0);
}

void Server::ExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd)
{
	if (this->channels[j].GetInvitOnly())
		{senderror(473, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :Cannot join channel (+i)\r\n"); return;}
	else if (this->channels[j].GetLimit() && this->channels[j].GetClientsNumber() >= this->channels[j].GetLimit())
		{senderror(471, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :Cannot join channel (+l)\r\n"); return;}
	else if (token[i].second.empty() && this->channels[j].GetPassword().empty()){
		if (this->channels[j].get_admin(fd) || this->channels[j].get_client(fd))
			{senderror(477, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :You are already in this channel\r\n"); return;}
	}
	else if (token[i].second == this->channels[j].GetPassword()){
		if (this->channels[j].get_admin(fd) || this->channels[j].get_client(fd))
			{senderror(477, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :You are already in this channel\r\n"); return;}
	}
	else if (token[i].second != this->channels[j].GetPassword())
		{senderror(475, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :Wrong password\r\n"); return;}
	this->channels[j].add_client(*GetClient(fd));
}

void Server::NotExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int fd)
{
	if (token[i].first[0] != '#' && token[i].first[0] != '&')
		{senderror(403, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :No such channel\r\n"); return;}
	Channel newChannel;
	newChannel.SetName(token[i].first);
	if (!token[i].second.empty())
		newChannel.SetPassword(token[i].second);
	newChannel.add_admin(*GetClient(fd));
	this->channels.push_back(newChannel);
}

void Server::JOIN(std::string cmd, int fd)
{
	std::vector<std::pair<std::string, std::string> > token;
	SplitJoin(token, cmd);
	for (size_t i = 0; i < token.size(); i++){
		for (size_t j = 0; j < this->channels.size(); j++){
			if (this->channels[j].GetName() == token[i].first)
				ExistCh(token, i, j, fd);
			else
				NotExistCh(token, i, fd);
		}
	}
}