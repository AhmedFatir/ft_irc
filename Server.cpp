/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khbouych <khbouych@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 10:06:56 by afatir            #+#    #+#             */
/*   Updated: 2024/01/28 23:48:48 by khbouych         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"

#define RPL_JOIN(nick, username, channelname, ipaddress) ":" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"

#define RPL_NAMREPLY(hostname, clients, channelname, nick) ":" + hostname + " 353 " + nick + " = " + channelname + " :" + clients + "\r\n"

#define RPL_ENDOFNAMES(hostname, nick, channelname) ":" + hostname + " 366 " + nick + " " + channelname + " :END of /NAMES list\r\n"


// int g_run;

// void signal_handler(int signal)
// {
//     (void)signal;
//     g_run = 0;
// }

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

	// g_run = 1;
	// std::signal(SIGINT, signal_handler);
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
	{
		size_t pos = line.find_first_of("\r\n");
		if(pos != std::string::npos)
			line = line.substr(0, pos);
		vec.push_back(line);
	}
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
		if(recived != "PONG localhost\r\n")
		{
			std::cout << "recived: " << recived;
			cmd = split_recivedBuffer(recived);
			for(size_t i = 0; i < cmd.size(); i++)
				this->parse_exec_cmd(cmd[i], fd);
		}
	}
}

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

void	Server::set_username(std::string& cmd, int fd)
{
	std::vector<std::string> splited_cmd = split_cmd(cmd);

	Client *cli = GetClient(fd); 
	if((cli && splited_cmd.size() < 5))
	{
		// ERR_NEEDMOREPARAMS (461): DONE
		std::string resp = ": 461 " + cli->GetNickName() + " USER :Not enough parameters\r\n";
		send(fd, resp.c_str(), resp.size(), 0);
	}
	else if(!cli || (cli && cli->GetNickName().empty()))
	{
		std::string resp = ": 541 nick :You have not registered\r\n";
		send(fd,resp.c_str(), resp.size(), 0);
		close(fd);
		RemoveFds(fd);
		RemoveClient(fd);
	}
	else if (cli && !cli->GetUserName().empty())
	{
		// ERR_ALREADYREGISTERED (462): DONE
		std::string resp = ": 462 " + cli->GetNickName() + " :You may not reregister\r\n";
		send(fd,resp.c_str(), resp.size(), 0);
	}
	else
	{
		cli->SetUsername(splited_cmd[1]);
		std::string welcome = ": 001 " + cli->GetNickName() + " :Welcome to the IRC server!\r\n";
		send(fd, welcome.c_str(), welcome.size(), 0);
	}
}


void Server::parse_exec_cmd(std::string &cmd, int fd)
{
	if(cmd.empty() || cmd == "\r")
		return ;
	std::vector<std::string> splited_cmd = split_cmd(cmd);
    if(splited_cmd[0] == "PASS")
        client_authen(fd, cmd);
	else if (splited_cmd[0] == "NICK")
		set_nickname(splited_cmd[1],fd);
	else if(splited_cmd[0] == "USER")
		set_username(cmd, fd);
	else if (splited_cmd[0] == "KICK")
		KICK(cmd, fd);
	else if (splited_cmd[0] == "JOIN")
		JOIN(cmd, fd);
	else if (splited_cmd[0] == "TOPIC")
		Topic(cmd, fd);
	else if (splited_cmd[0] == "MODE")
		mode_command(cmd, fd);

}


bool Server::is_validNickname(std::string& nickname)
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

void Server::set_nickname(std::string& nickname, int fd)
{
	if(nickname == ":" || nickname.empty())
	{
		// ERR_NONICKNAMEGIVEN (431)
		std::string resp = ": 431 user :No nickname given\r\n";
		if(send(fd,resp.c_str(), resp.size(), 0) == -1)
			throw(std::runtime_error("send() faild"));
	}
	else if (nickNameInUse(nickname))
	{
		//ERR_NICKNAMEINUSE (433)
		std::string resp = ":localhost 433 user :Nickname is already in use\r\n";
		if(send(fd,resp.c_str(), resp.size(), 0) == -1)
			throw(std::runtime_error("send() faild"));
	}
	else if(!is_validNickname(nickname))
	{
		//ERR_ERRONEUSNICKNAME (432)
		std::string resp = ": 432 user :Erroneus nickname\r\n";
		send(fd,resp.c_str(), resp.size(), 0);
	}
	else
	{
		Client *cli = GetClient(fd);
		if(cli)
		{
			// ":" + oldNick + " NICK " + nick + "\r\n"
			std::string oldNick = cli->GetNickName();
			if(!oldNick.empty())
			{
				std::string resp = ":" + oldNick + " NICK " + nickname + "\r\n";
				std::cout << resp; 
				send(fd, resp.c_str(), resp.size(), 0);
			}
			cli->SetNickname(nickname);
		}
		else	
		{
			std::string resp = ": 541 " + nickname + " :You have not registered\r\n";
			send(fd,resp.c_str(), resp.size(), 0);
			close(fd);
			RemoveFds(fd);
			RemoveClient(fd);
		}
	}
}



bool Server::is_clientExist(int fd)
{
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (this->clients[i].GetFd() == fd)
			return true;
	}
	return false;
}
bool Server::nickNameInUse(std::string& nickname)
{
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (this->clients[i].GetNickName() == nickname)
			return true;
	}
	return false;
}

void Server::client_authen(int fd, std::string& cmd)
{
	Client cli;
	std::vector<std::string> splited_cmd = split_cmd(cmd);
	if(splited_cmd.size() < 2)
	{
		std::string resp = ":localhost 461 user :Not enough parameters\r\n";
		send(fd, resp.c_str(), resp.size(), 0);
	}
	else if(!is_clientExist(fd))
	{
		std::string pass = splited_cmd[1];
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
		}
	}
	else
	{
		std::string resp = ":localhost 642 user :You may not reregister\r\n";
		send(fd, resp.c_str(), resp.size(),0);

	}
}
//####################################JOIN##################################################
void SplitCmdJoin(std::string cmd, std::vector<std::string> &tmp)
{
	// cmd = cmd.substr(0, cmd.size() - 1);
	std::string str;
	for (size_t i = 0; i < cmd.size(); i++){
		if (cmd[i] == ' ' || cmd[i] == ',')
			{tmp.push_back(str); str.clear(); while(cmd[i] == ' ') i++; i--;}
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

void senderror(int code, std::string clientname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << msg;
	std::string resp = ss.str();
	send(fd, resp.c_str(), resp.size(),0);
}

void senderror(int code, std::string clientname, std::string channelname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << " " << channelname << msg;
	std::string resp = ss.str();
	send(fd, resp.c_str(), resp.size(),0);
}


// std::string Server::get_clientList(Channel &channel)
// {
// 	std::string clinets;
// 	for(size_t i = 0; i < channel.clientChannel_size(); i++)
// 	{
// 		clinets += cha
// 	}
// }

int Server::SearchForClients(std::string nickname)
{
	int count = 0;
	for (size_t i = 0; i < this->channels.size(); i++){
		if (this->channels[i].GetClientInChannel(nickname))
			count++;
	}
	return count;
}

void Server::ExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd)
{
	/*
	ERR_BANNEDFROMCHAN (474) // if the client is banned from the channel
	ERR_BADCHANMASK (476) // if the channel mask is invalid
	*/
	
	if (SearchForClients(GetClient(fd)->GetNickName()) >= 10)//ERR_TOOMANYCHANNELS (405) // if the client is already in 10 channels
		{senderror(405, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :You have joined too many channels\r\n"); return;}
	if (this->channels[j].GetInvitOnly())// ERR_INVITEONLYCHAN (473) // if the channel is invit only
		{senderror(473, GetClient(fd)->GetNickName(), token[i].first, GetClient(fd)->GetFd(), " :Cannot join channel (+i)\r\n"); return;}
	if (this->channels[j].GetLimit() && this->channels[j].GetClientsNumber() >= this->channels[j].GetLimit())// ERR_CHANNELISFULL (471) // if the channel reached the limit of number of clients
		{senderror(471, GetClient(fd)->GetNickName(), token[i].first, GetClient(fd)->GetFd(), " :Cannot join channel (+l)\r\n"); return;}
	if (!this->channels[j].GetPassword().empty() && this->channels[j].GetPassword() != token[i].second)// ERR_BADCHANNELKEY (475) // if the password is incorrect
		{senderror(475, GetClient(fd)->GetNickName(), token[i].first, GetClient(fd)->GetFd(), " :Cannot join channel (+k) - bad key\r\n"); return;}
	if (this->channels[j].GetClientInChannel(GetClient(fd)->GetNickName()))// ERR_ALREADYREGISTRED (462) // if the client is already registered
		{senderror(462, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :You may not reregister\r\n"); return;}
	// add the client to the channel
	Client *cli = GetClient(fd);
	this->channels[j].add_client(*cli);
	std::string respo = ":" + cli->GetNickName() + "!" + cli->GetUserName() + "@localhost JOIN #" + token[i].first + "\r\n";
	std::string resp2 = ": 353 " + cli->GetNickName() + " @ #" + token[i].first + " :" + channels[j].clientChannel_list() + "\r\n";
	std::string resp3 = ": 366 " + cli->GetNickName() + " #" + token[i].first + " :END of /NAMES list.\r\n";
	channels[i].sendTo_all(respo, resp2, resp3);
}


void Server::NotExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int fd)
{
	Channel newChannel;
	newChannel.SetName(token[i].first);
	if (!token[i].second.empty())
		newChannel.SetPassword(token[i].second);
	newChannel.add_admin(*GetClient(fd));
	this->channels.push_back(newChannel);
	// notifiy thet the client joined the channel
	std::string respo = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost JOIN #" + token[i].first + "\r\n"; // join message
	std::string resp2 = ": 353 " + GetClient(fd)->GetNickName() + " @ #" + token[i].first + " :@" + GetClient(fd)->GetNickName() + "\r\n"; // RPL_NAMREPLY (353) Messages:
	std::string resp3 = ": 366 " + GetClient(fd)->GetNickName() + " #" + token[i].first + " :END of /NAMES list.\r\n"; // RPL_ENDOFNAMES (366) Message:
	std::cout << respo << resp2 << resp3; 
	send(fd, respo.c_str(), respo.size(),0);
	send(fd, resp2.c_str(), resp2.size(),0);
	send(fd, resp3.c_str(), resp3.size(),0);
}

void Server::JOIN(std::string cmd, int fd)
{
	if (cmd.size() < 6)// ERR_NEEDMOREPARAMS (461) // if the channel name is empty
		{senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	std::vector<std::pair<std::string, std::string> > token;
	SplitJoin(token, cmd);
	for (size_t i = 0; i < token.size(); i++){//ERR_NOSUCHCHANNEL (403) // if the channel doesn't exist
		if (*(token[i].first.begin()) != '#' && *(token[i].first.begin()) != '&')
			{senderror(403, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :No such channel\r\n"); return;}
		else
			token[i].first.erase(token[i].first.begin());
	}
	for (size_t i = 0; i < token.size(); i++){
		bool flag = false;
		for (size_t j = 0; j < this->channels.size(); j++){
			if (this->channels[j].GetName() == token[i].first){
				ExistCh(token, i, j, fd);
				flag = true; break;
			}
		}
		if (!flag)
			NotExistCh(token, i, fd);
	}
}

void	Server::close_fds()
{
		for(size_t i = 0; i < clients.size(); i++)
		{
			std::cout << "close: " << i << std::endl;
			close(clients[i].GetFd());
		}
		close(server_fdsocket);
}
//####################################KICK##################################################
void SplitCmdKick(std::string cmd, std::vector<std::string> &tmp)
{
	std::istringstream stm(cmd);
	std::string str;
	while(stm >> cmd)
		tmp.push_back(cmd);
	tmp.erase(tmp.begin());
	for (size_t i = 2; i < tmp.size(); i++){//take just the first 2 strings and rest of the string is the reason
			for (size_t j = i; j < tmp.size(); j++)
				{str += " " + tmp[j];tmp.erase(tmp.begin() + j);j--;}
	}
	if (!str.empty()) // if the reason is not empty add it
		tmp.push_back(str);
	//erase the '#' from the channel name
	tmp[0].erase(tmp[0].begin());
}

void	Server::KICK(std::string cmd, int fd)
{
	//ERR_BADCHANMASK (476) // if the channel mask is invalid
	std::vector<std::string> tmp;
	SplitCmdKick(cmd, tmp);
	int flag = 0;
	if (tmp.size() < 2) // check if the client send the channel name and the client to kick
		{senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	for (size_t i = 0; i < this->channels.size(); i++){ // search for the channel
		if (this->channels[i].GetName() == tmp[0]){ // check if the channel exist
			flag = 1;
			if (!channels[i].get_client(fd) && !channels[i].get_admin(fd)) // check if the client is in the channel
				{senderror(442, GetClient(fd)->GetNickName(), tmp[0], GetClient(fd)->GetFd(), " :You're not on that channel\r\n"); return;}
			if(this->channels[i].get_admin(fd)){ // check if the client is admin
				if (channels[i].GetClientInChannel(tmp[1])){ // check if the client to kick is in the channel
					std::stringstream ss;
					ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName() << "@" << "localhost" << " KICK #" << tmp[0] << " " << tmp[1];
					if (tmp.size() == 3)
						ss << " :" <<tmp[2] << "\r\n";
					else ss << "\r\n";
					std::string resp = ss.str();
					this->channels[i].sendTo_all(resp);
					std::cout << "		" << resp;
					if (channels[i].get_admin(channels[i].GetClientInChannel(tmp[1])->GetFd()))
						channels[i].remove_admin(channels[i].GetClientInChannel(tmp[1])->GetFd());
					else
						channels[i].remove_client(channels[i].GetClientInChannel(tmp[1])->GetFd());
				}
				else // if the client to kick is not in the channel
					{senderror(441, GetClient(fd)->GetNickName(), tmp[0], GetClient(fd)->GetFd(), " :They aren't on that channel\r\n"); return;}
			}
			else // if the client is not admin
				{senderror(482, GetClient(fd)->GetNickName(), tmp[0], GetClient(fd)->GetFd(), " :You're not channel operator\r\n"); return;}
		}
	}
	if (flag == 0) // if the channel doesn't exist
		senderror(403, GetClient(fd)->GetNickName(), tmp[0], GetClient(fd)->GetFd(), " :No such channel\r\n");
}


//############# PLEASE DON'T REMOVE THIS COMMENT

// (void)cmd;
// std::string host = "localhost";
// std::string chnam = "#cc";
// std::string nickname = GetClient(fd)->GetNickName();
// std::string username = GetClient(fd)->GetUserName();

// // :aa!~oo@localhost JOIN #cc
// std::string resp = ":localhost JOIN " + nickname + " " + chnam + "\r\n";

// // RPL_TOPIC (332) Message (Channel with No Topic): // :irc.server.com 332 dan- #new_channel :
// std::string resp1 = ":localhost 332 " + nickname + " " + chnam + " :Welcome to the channel!" + "\r\n";

// // :localhost 353  aa = #cc : aa // :irc.server.com 353 dan- = #test :@dan- john
// std::string resp2 = ":localhost 353 " + nickname + " = " + chnam + " :@" + nickname + "\r\n";

// // ":" + hostname + " 366 " + nick + " " + channelname + " :END of /NAMES list\r\n"
// std::string resp3 = ":localhost 366 " + nickname + " " + chnam + " :END of NAMES list" + "\r\n";

// send(fd, resp.c_str(), resp.size(),0);
// send(fd, resp1.c_str(), resp1.size(),0);
// send(fd, resp2.c_str(), resp2.size(),0);
// send(fd, resp3.c_str(), resp3.size(),0);
// std::cout << resp; // join message 
// std::cout << resp1; // RPL_TOPIC (332) Message:
// std::cout << resp2; // RPL_NAMREPLY (353) Messages:
// std::cout << resp3; // RPL_ENDOFNAMES (366) Message:


//------------------------TOPIC--------------------------------------------------------------


bool Server::checkifadmin(int &fd)
{
	for(size_t i=0; i < channels.size(); i++)
		if (channels[i].get_admin(fd))
			return true;
	return false;
}

Channel *Server::GetChannel(std::string name)
{
	for (size_t i = 0; i < this->channels.size(); i++){
		if (this->channels[i].GetName() == name)
			return &channels[i];
	}
	return NULL;
}

std::string Server::getnamechannel(std::string &name)
{
	if (!name.empty() && name[0] == '#')
		return (name.substr(1));
	return name;
}

bool Server::checkifchannelexist(std::string &namechannel)
{
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (namechannel == (channels[i].GetName()))
			return true;
	}
		return false;
}

// Channel *Server::getchannel(std::string name , std::vector<Channel> lstchannel)
// {
// 	for (int i = 0; i < (int)lstchannel.size(); i++)
// 	{
// 		if (lstchannel[i].GetName() == name)
// 			return lstchannel[i];
// 	}
// 	return NULL;
// }

// Channel* Server::GetChannelByName(const std::string& name)
// {
//     for (size_t i = 0; i < this->channels.size(); i++) {
//         if ( == name)
// 		{
// 			std::cout << "ana hena " << channels[i].GetName() << std::endl;
//             return &this->channels[i];
// 		}
//     }
//     return NULL;
// }

std::string Server::tTopic()
{
	std::time_t current = std::time(NULL);
	std::stringstream res;

	res << current;
	return res.str();
}

// std::string	Server::_printMessage(std::string num, std::string nickname, std::string message)
// {
// 	if (nickname.empty())
// 		nickname = "*";
// 	return (":" + this->_name + " " + num + " " + nickname + " " + message + "\n");
// }

void Server::Topic(std::string &cmd, int &fd)
{
	(void)fd;
	std::vector<std::string> scmd = split_cmd(cmd);
	std::string nmch = getnamechannel(scmd[1]);
	Channel ch;
	int chexist = 0;
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (nmch == channels[i].GetName())
		{
			ch = channels[i];
			chexist = 1;
			break;
		}
		chexist = 0;
	}
	if (chexist)
	{
		Client *admin = ch.get_admin(fd);
		if (admin)
		{
			ch.SetTopicName(scmd[2]);
			std::string respons= ":" + admin->GetNickName() + "!" + admin->GetUserName() + "@localhost TOPIC #" + nmch + " :" + ch.GetTopicName() + "\r\n";
			send(fd, respons.c_str(), respons.size(),0);

			// if 
			// std::string rep = ":localhost 332 " + admin->GetNickName() + " " + scmd[1] + " :" + ch.GetTopicName() + "\r\n";
			// send(fd, rep.c_str(), rep.length(), 0);

			// std::string rep1 = ":localhost 333 " + admin->GetNickName() + " " + scmd[1] + " " + admin->GetNickName() + " " + tTopic() + "\r\n";
			// send(fd, rep1.c_str(), rep1.length(), 0);
			// std::cout << rep << rep1;
		}
		else
		{
			std::string respons = ":localhost 482 " + GetClient(fd)->GetNickName() + " " + nmch + " :You're not channel operator\r\n";
			send(fd, respons.c_str(), respons.size(),0);
		}
	}
	else
	{
		std::string respons = ":localhost 403 " + GetClient(fd)->GetNickName() + " " + nmch + " :No such channel\r\n";
		send(fd, respons.c_str(), respons.size(),0);
	}
}
//################################PART#####################################################
std::string SplitCmdPart(std::string cmd, std::vector<std::string> &tmp)
{
	std::istringstream stm(cmd);
	std::string reason;
	while(stm >> cmd)
		tmp.push_back(cmd);
	tmp.erase(tmp.begin());
	for (size_t i = 1; i < tmp.size(); i++){//start from the second string and take the rest of the strings as the reason
			for (size_t j = i; j < tmp.size(); j++)
				{reason += " " + tmp[j];tmp.erase(tmp.begin() + j);j--;}
	}
	std::string str = tmp[0]; std::string str1; tmp.clear();
	for (size_t i = 0; i < str.size(); i++){//split the first string by ',' to get the channels names
		if (str[i] == ',')
			{tmp.push_back(str1); str1.clear();}
		else str1 += str[i];
	}
	tmp.push_back(str1);
	for (size_t i = 0; i < tmp.size(); i++)//erase the empty strings
		{if (tmp[i].empty())tmp.erase(tmp.begin() + i);}
	for (size_t i = 0; i < tmp.size(); i++)// erase the '#' from the channel name
		{if (*(tmp[i].begin()) == '#' || *(tmp[i].begin()) == '&') tmp[i].erase(tmp[i].begin());}
	return reason;
}

void Server::PART(std::string cmd, int fd)
{
	if (cmd.size() < 6)// ERR_NEEDMOREPARAMS (461) // if the channel name is empty
		{senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	std::vector<std::string> tmp;
	std::string reason = SplitCmdPart(cmd, tmp);
	for (size_t i = 0; i < tmp.size(); i++){
		bool flag = false;
		for (size_t j = 0; j < this->channels.size(); j++){ // search for the channel
			if (this->channels[j].GetName() == tmp[i]){ // check if the channel exist
				flag = true;
				if (!channels[j].get_client(fd) && !channels[j].get_admin(fd)) // ERR_NOTONCHANNEL (442) // if the client is not in the channel
					{senderror(442, GetClient(fd)->GetNickName(), tmp[i], GetClient(fd)->GetFd(), " :You're not on that channel\r\n"); return;}
					std::stringstream ss;
					ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName() << "@" << "localhost" << " PART #" << tmp[i];
					if (!reason.empty())
						ss << " :" << reason << "\r\n";
					else ss << "\r\n";
					std::string resp = ss.str();
					send(fd, resp.c_str(), resp.size(),0);
					std::cout << "		" << resp;
					if (channels[j].get_admin(channels[j].GetClientInChannel(GetClient(fd)->GetNickName())->GetFd()))
						channels[j].remove_admin(channels[j].GetClientInChannel(GetClient(fd)->GetNickName())->GetFd());
					else
						channels[j].remove_client(channels[j].GetClientInChannel(GetClient(fd)->GetNickName())->GetFd());
			}
		}
		if (!flag) // ERR_NOSUCHCHANNEL (403) // if the channel doesn't exist
			senderror(403, GetClient(fd)->GetNickName(), tmp[i], GetClient(fd)->GetFd(), " :No such channel\r\n");
	}
}

//################################ MODE #####################################################


std::string mode_toAppend(std::string chain, char opera, char mode)
{
	std::stringstream ss;

	ss.clear();
	char last = '\0';
	for(size_t i = 0; i < chain.size(); i++)
	{
		if(chain[i] == '+' || chain[i] == '-')
			last = chain[i];
	}
	if(last != opera)
		ss << opera << mode;
	else
		ss << mode;
	return ss.str();
}

void Server::mode_command(std::string& cmd, int fd)
{

	//if channel is not exsit ERR_NOSUCHCHANNEL (403) is returned
	//if <options> is not given, the RPL_CHANNELMODEIS (324) numeric is returned. (MODE #example)
	// If a user does not have appropriate privileges to change modes on the target channel, \
	// the server MUST NOT process the message, and ERR_CHANOPRIVSNEEDED (482) numeric is returned

	std::vector<std::pair<char, bool> > modes;
	char charaters[5] = {'i', 't', 'k', 'o', 'l'};
	for(int i = 0; i < 5; i++)
		modes.push_back(std::make_pair(charaters[i],false));
	std::string pass;
	std::string _pass;
	std::string user;
	std::string limit;

	std::vector<std::string> splited = split_cmd(cmd);
	char opera = '\0';
	std::stringstream mode_chain;
	Client *cli = GetClient(fd);
	if(!cli)
	{
		std::string resp = ": 451 nickname :You have not registered\r\n";
		send(fd,resp.c_str(), resp.size(), 0);
		close(fd);
		RemoveFds(fd);
		RemoveClient(fd);
		return ;
	}
	else if(splited.size() < 2)
	{
		std::string resp = ": 461 " + cli->GetNickName() + " USER :Not enough parameters\r\n";
		send(fd, resp.c_str(), resp.size(), 0);
		return ;
	}
	std::string channelName = splited[1].substr(1);
	Channel *channel = GetChannel(channelName);

	if(!channel)
	{
		// "<client> <channel> :No such channel"
		std::string resp = ": 403 " + cli->GetNickName()+  " " + channelName + " No such channel\r\n";
		send(fd, resp.c_str(), resp.size(), 0);
		return ;
	}
	else if(channel)
	{
		size_t pos = 3;
		if (splited.size() == 2)
			std::cout << "reply with modes of channel" << std::endl;
		else
		{
			for(size_t i = 0; i < splited[2].size(); i++)
			{
				std::cout << "cmd: " << splited[2][i] << std::endl;
				if(splited[2][i] == '+' || splited[2][i] == '-')
					opera = splited[2][i];
				else
				{
					if(splited[2][i] == 'i')
					{
						if(opera == '+')
						{
							mode_chain <<  mode_toAppend(mode_chain.str(), opera, splited[2][i]);
							modes[0].second = true;
						}
						else if (opera == '-')
						{
							mode_chain << mode_toAppend(mode_chain.str(), opera, splited[2][i]);
							modes[0].second = false;
						}
					}
				
					else if (splited[2][i] == 't')
					{
						if(opera == '+')
						{
							mode_chain <<  mode_toAppend(mode_chain.str(), opera, splited[2][i]);
							modes[1].second = true;
						}
						else if (opera == '-')
						{
							mode_chain <<  mode_toAppend(mode_chain.str(), opera, splited[2][i]);
							modes[1].second = false;
						}
					}
					else if (splited[2][i] == 'k')
					{	
						if(opera == '+')
						{
							if(splited.size() > pos)
							{
								modes[2].second = true;
								pass = splited[pos++];
								mode_chain <<  mode_toAppend(mode_chain.str(), opera, splited[2][i]);
							}
							else
							{
								// ":" + hostname + " 324 " + nick + " " + channelName + " " + channelmode + "\r\n"
								std::string resp = ": 324 " + GetClient(fd)->GetNickName() + " " + channel->GetName() + " +k\r\n"; 
								std::cout << "You must specify a parameter for the key mode (+k)\n";
								send(fd, resp.c_str(), resp.size(), 0);
							}
						}
						else if (opera == '-')
						{
							if(splited.size() > pos)
							{
								modes[2].second = false;
								pass = splited[pos++];
								mode_chain <<  mode_toAppend(mode_chain.str(), opera, splited[2][i]);
							}
							else
								std::cout << "You must specify a parameter for the key mode(-k)";
						}
					}
					else if (splited[2][i] == 'o')
					{	
						if(opera == '+')
						{
							if(splited.size() > pos)
							{
								modes[3].second = true;
								pass = splited[pos++];
								mode_chain <<  mode_toAppend(mode_chain.str(), opera, splited[2][i]);
							}
							else
								std::cout << "You must specify a parameter for the key mode (+o)";
						}
						else if (opera == '-')
						{
							if(splited.size() > pos)
							{
								modes[3].second = false;
								_pass = splited[pos++];
								mode_chain <<  mode_toAppend(mode_chain.str(), opera, splited[2][i]);
							}
							else
								std::cout << "You must specify a parameter for the key mode (-o)";
						}
					}
					else if (splited[2][i] == 'l')
					{	
						if(opera == '+')
						{
							if(splited.size() > pos)
							{
								modes[4].second = true;
								limit = splited[pos++];
								mode_chain <<  mode_toAppend(mode_chain.str(), opera, splited[2][i]);
							}
							else
								std::cout << "You must specify a parameter for the key mode (+l)";
						}
						else if (opera == '-')
						{
							if(splited.size() > pos)
							{
								modes[4].second = false;
								limit = splited[pos++];
								mode_chain <<  mode_toAppend(mode_chain.str(), opera, splited[2][i]);
							}
							else
								std::cout << "You must specify a parameter for the key mode (-l)";
						}
					}
				}
			}
		}
	}

	for(size_t i = 0; i < modes.size(); i++)
	{
		if(modes[i].first == 'k')
			std::cout << ":" << '\'' << modes[i].first << '\'' << modes[i].second << " PASS: " << pass <<  "| " <<  _pass << std::endl;
		else if (modes[i].first == 'o') 
			std::cout << ":" << '\'' << modes[i].first << '\'' << modes[i].second << " USER: " << user << std::endl;
		else if (modes[i].first == 'l') 
			std::cout << ":" << '\'' << modes[i].first << '\'' << modes[i].second << " LIMIT: " << limit << std::endl;
		else
			std::cout << ":" << '\'' << modes[i].first << '\'' << modes[i].second << std::endl;
	}
	// std::cout << "mode chain: " << mode_chain.str() << std::endl;
	// std::string resp = ":" + cli->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost MODE " + channel->GetName() +" " + mode_chain.str() + "\r\n";
	// send(fd, resp.c_str(), resp.size(),0);
	// std::cout << resp;

}
