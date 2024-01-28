/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khbouych <khbouych@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 10:06:56 by afatir            #+#    #+#             */
/*   Updated: 2024/01/28 17:07:31 by khbouych         ###   ########.fr       */
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

void senderror(int code, std::string clientname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << msg;
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

void Server::ExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd)
{
	// if the channel is invit only
	// if (this->channels[j].GetInvitOnly())
	// {
	// 	std::cout <<" 1 \n";
	// 	senderror(473, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :Cannot join channel (+i)\r\n"); return;
	// } 
	// // channel reached the limit of number of clients
	// else if (this->channels[j].GetLimit() && this->channels[j].GetClientsNumber() >= this->channels[j].GetLimit())
	// {
	// 	std::cout <<" 2 \n";
	// 	senderror(471, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :Cannot join channel (+l)\r\n"); return;
	// }
	// //
	// else if (token[i].second.empty() && this->channels[j].GetPassword().empty()){
	// 	std::cout <<" 3 \n";
	// 	if (this->channels[j].get_admin(fd) || this->channels[j].get_client(fd))
	// 		{senderror(477, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :You are already in this channel\r\n"); return;}
	// }
	// else if (token[i].second == this->channels[j].GetPassword()){
	// 	std::cout <<" 4 \n";
	// 	if (this->channels[j].get_admin(fd) || this->channels[j].get_client(fd))
	// 		{senderror(477, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :You are already in this channel\r\n"); return;}
	// }
	// else if (token[i].second != this->channels[j].GetPassword())
	// {
	// 	std::cout << " i was here uuuuuuuuuu\n";
	// 	senderror(475, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :Wrong password\r\n"); return;
	// }
	// else
	// {
			Client *cli = GetClient(fd);
			this->channels[j].add_client(*cli);
			std::ofstream outputFile("out");
			// std::cout << channels[j].clientChannel_list() << "|"<< std::endl;

			std::string respo = ":" + cli->GetNickName() + "!" + cli->GetUserName() + "@localhost JOIN #" + token[i].first + "\r\n";
			std::string resp2 = ": 353 " + cli->GetNickName() + " @ #" + token[i].first + " :" + channels[j].clientChannel_list() + "\r\n";
			std::string resp3 = ": 366 " + cli->GetNickName() + " #" + token[i].first + " :END of /NAMES list.\r\n";
			channels[i].sendTo_all(respo, resp2, resp3);

			// std::string cli_list = channels[j].clientChannel_list();

			outputFile << respo;
			outputFile << resp2;
			outputFile << resp3;
			// std::cout << respo << resp2 << resp3 << std::endl;
			// send(fd, respo.c_str(), respo.size(),0);
			// send(fd, resp2.c_str(), resp2.size(),0);
			// send(fd, resp3.c_str(), resp3.size(),0);
		
	// }
}


void Server::NotExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int fd)
{
	// if (token[i].first[0] != '#' && token[i].first[0] != '&')
	// 	{senderror(403, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :No such channel\r\n"); return;}
	Channel newChannel;
	newChannel.SetName(token[i].first);
	if (!token[i].second.empty())
		newChannel.SetPassword(token[i].second);
	newChannel.add_admin(*GetClient(fd));
	this->channels.push_back(newChannel);
	// notifiy thet the client joined the channel
	///////////////////////////////////////////////////
	// ABDO >> this will be recoded to more clean code
	//////////////////////////////////////////////////
	Client *cli = GetClient(fd);
	std::string respo = ":" + cli->GetNickName() + "!" + cli->GetUserName() + "@localhost JOIN #" + token[i].first + "\r\n";
	std::string resp2 = ": 353 " + cli->GetNickName() + " @ #" + token[i].first + " :@" + cli->GetNickName() + "\r\n";
	std::string resp3 = ": 366 " + cli->GetNickName() + " #" + token[i].first + " :END of /NAMES list.\r\n";

	std::cout << respo; // join message
	std::cout << resp2; // RPL_NAMREPLY (353) Messages:
	std::cout << resp3; // RPL_ENDOFNAMES (366) Message:

	send(fd, respo.c_str(), respo.size(),0);
	send(fd, resp2.c_str(), resp2.size(),0);
	send(fd, resp3.c_str(), resp3.size(),0);
	std::cout << std::endl;
}

void Server::JOIN(std::string cmd, int fd)
{
	// (void)cmd;
	
	// std::string host = "localhost";
	// std::string chnam = "#cc";
	// std::string nickname = GetClient(fd)->GetNickName();
	// std::string username = GetClient(fd)->GetUserName();
	// std::string clis = "@" + nickname;

	// std::cout << std::endl;
	// std::cout << "host " << host << std::endl;
	// std::cout << "chnam " << chnam << std::endl;
	// std::cout << "nickname " << nickname << std::endl;
	// std::cout << "username " << username << std::endl;
	// std::cout << "clis " << clis << std::endl;
	// // ":" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"
	// std::cout << std::endl;

	// std::string respo = ":" + nickname + "!" + username + "@localhost JOIN " + chnam + "\r\n";


	// std::string resp2 = ": 353 " + nickname + " @ " + chnam + " :@" + nickname + "\r\n";

	// // // ":" + hostname + " 366 " + nick + " " + channelname + " :END of /NAMES list\r\n"
	// std::string resp3 = ": 366 " + nickname + " " + chnam + " :END of /NAMES list.\r\n";

	// std::cout << respo; // join message 
	// std::cout << resp2; // RPL_NAMREPLY (353) Messages:
	// std::cout << resp3; // RPL_ENDOFNAMES (366) Message:

	// send(fd, respo.c_str(), respo.size(),0);
	// send(fd, resp2.c_str(), resp2.size(),0);
	// send(fd, resp3.c_str(), resp3.size(),0);

	std::vector<std::pair<std::string, std::string> > token;
	SplitJoin(token, cmd);
	for (size_t i = 0; i < token.size(); i++)
		token[i].first.erase(token[i].first.begin());
	for (size_t i = 0; i < token.size(); i++)
	{
		bool flag = false;
		for (size_t j = 0; j < this->channels.size(); j++)
		{
			if (this->channels[j].GetName() == token[i].first){
				std::cout << "i was here kakaka \n";
				ExistCh(token, i, j, fd);
				flag = true; break;
			}
		}
		if (!flag)
			NotExistCh(token, i, fd);
	}
}
///
void	Server::close_fds()
{
		for(size_t i = 0; i < clients.size(); i++)
		{
			std::cout << "close: " << i << std::endl;
			close(clients[i].GetFd());
		}
		close(server_fdsocket);
}
//######################################################################################
//functions to kick a client
void SplitCmdKick(std::string cmd, std::vector<std::string> &tmp)
{
	std::string str;
	for (size_t i = 0; i < cmd.size(); i++){
		if (cmd[i] == ':')
			{tmp.push_back(cmd.substr(i));tmp.erase(tmp.begin());return;};
		if (cmd[i] == ' ')
			{tmp.push_back(str); str.clear();}
		else
			str += cmd[i];
	}
	tmp.push_back(str);
	tmp.erase(tmp.begin());
}

void	Server::KICK(std::string cmd, int fd)
{
	std::vector<std::string> tmp;
	SplitCmdKick(cmd, tmp);
	// for(int i = 0; i < )
	std::cout << tmp[0] << std::endl;
	exit(0);
	tmp[0].erase(tmp[0].begin());
	int flag = 0;
	if (tmp.size() < 2)
		{senderror(461, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	for (size_t i = 0; i < this->channels.size(); i++){
		if (this->channels[i].GetName() == tmp[0]){
			flag = 1;
			if (!channels[i].get_client(fd) && !channels[i].get_admin(fd))
				{senderror(442, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :You're not on that channel\r\n"); return;}
			//check if the client is admin
			if(this->channels[i].get_admin(fd)){
				//check if the client name is in the channel
				if (channels[i].GetClientInChannel(tmp[1])){
					//check if the client is admin
					if (channels[i].get_admin(channels[i].GetClientInChannel(tmp[1])->GetFd()))
						channels[i].remove_admin(channels[i].GetClientInChannel(tmp[1])->GetFd());
					else
						channels[i].remove_client(channels[i].GetClientInChannel(tmp[1])->GetFd());
					//send the msg to the channel
					std::stringstream ss;
					ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName() << "@" << "localhost" << " KICK #" << tmp[0] << " " << tmp[1];
					if (tmp.size() == 3)
						ss << tmp[2] << "\r\n";
					else ss << "\r\n";
					std::string resp = ss.str();
					std::cout << "		" << resp;
				}
				else
					{senderror(441, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :They aren't on that channel\r\n"); return;}
			}
			else
				{senderror(482, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :You're not channel operator\r\n"); return;}
		}
	}
	if (flag == 0)
		senderror(403, GetClient(fd)->GetUserName(), GetClient(fd)->GetFd(), " :No such channel\r\n");
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

Channel* Server::GetChannelByName(const std::string& name)
{
    for (size_t i = 0; i < this->channels.size(); i++) {
		std::cout << "name " << channels[i].GetName()  <<"|"<< std::endl;
        if (this->channels[i].GetName() == name)
		{
			std::cout << "ana hena " << channels[i].GetName() << std::endl;
            return &this->channels[i];
		}
    }
    return NULL;
}

void Server::Topic(std::string &cmd, int &fd)
{
	(void)fd;
	std::vector<std::string> scmd = split_cmd(cmd);
	std::string nmch = getnamechannel(scmd[1]);
	Channel *ch = GetChannelByName(nmch);
	if(ch)
		std::cout << "nmch " << ch->GetName() << std::endl;
	else 
		std::cout << "nmch |" << nmch  << "|"<< std::endl;
}