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
Client *Server::GetClientNick(std::string nickname){
	for (size_t i = 0; i < this->clients.size(); i++){
		if (this->clients[i].GetNickName() == nickname)
			return &this->clients[i];
	}
	return NULL;
}
//######################################CLOSE
void	Server::close_fds()
{
		for(size_t i = 0; i < clients.size(); i++)
		{
			std::cout << "close index: " << i << " /NumFd: "<< clients[i].GetFd() << std::endl;
			close(clients[i].GetFd());
		}
		std::cout << "close the server's Fd: " << server_fdsocket << std::endl;
		close(server_fdsocket);
}

bool Server::Signal = false;

void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "SignalHandler" << std::endl;
	Server::Signal = true;
}

void Server::init(int port, std::string pass)
{
	std::string recived;
	this->password = pass;
	this->port = port;
	this->set_sever_socket();

	std::cout << "Waiting to accept a connection...\n";
	while (Server::Signal == false)
	{
		if(poll(&fds[0],fds.size(),-1) == -1)
			std::cout << "poll() faild or signal recived" << std::endl;
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents == POLLIN)
			{
				if (fds[i].fd == server_fdsocket){
					this->accept_new_client();
				}
				else {
					this->accept_new_message(fds[i].fd, recived);
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

void Server::accept_new_message(int fd, std::string &recived)
{
	char buff[1024];
	ssize_t bytes;
	std::vector<std::string> cmd;
	// std::string recived;
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
		recived += buff;
		if(recived.find_first_of("\r\n") == std::string::npos)
			return;
		if(recived != "PONG localhost\r\n")
		{
			std::cout << "recived: " << recived;
			cmd = split_recivedBuffer(recived);
			for(size_t i = 0; i < cmd.size(); i++)
				this->parse_exec_cmd(cmd[i], fd);
			recived.clear();
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
	else if (splited_cmd[0] == "PART")
		PART(cmd, fd);
	else if (splited_cmd[0] == "PRIVMSG")
		PRIVMSG(cmd, fd);
	else if (splited_cmd[0] == "INVITE")
		Invite(cmd,fd);
	else if (splited_cmd[0] == "QUIT")
		QUIT(cmd,fd);

}

// bool Server::is_validNickname(std::string& nickname)
// {
// 	if(!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':' || std::isdigit(nickname[0])))
// 		return false;
// 	for(size_t i = 1; i < nickname.size(); i++)
// 	{
// 		if(!std::isalnum(nickname[i]) && nickname[i] != '{' && nickname[i] != '}' && \
// 			nickname[i] != '[' && nickname[i] != ']' && nickname[i] != '|' && nickname[i] != '\\')
// 			return false;
// 	}
// 	return true;
// }

// void Server::set_nickname(std::string& nickname, int fd)
// {
// 	if(nickname == ":" || nickname.empty())
// 	{
// 		// ERR_NONICKNAMEGIVEN (431)
// 		std::string resp = ": 431 user :No nickname given\r\n";
// 		if(send(fd,resp.c_str(), resp.size(), 0) == -1)
// 			throw(std::runtime_error("send() faild"));
// 	}
// 	else if (nickNameInUse(nickname))
// 	{
// 		//ERR_NICKNAMEINUSE (433)
// 		std::string resp = ":localhost 433 user :Nickname is already in use\r\n";
// 		if(send(fd,resp.c_str(), resp.size(), 0) == -1)
// 			throw(std::runtime_error("send() faild"));
// 	}
// 	else if(!is_validNickname(nickname))
// 	{
// 		//ERR_ERRONEUSNICKNAME (432)
// 		std::string resp = ": 432 user :Erroneus nickname\r\n";
// 		send(fd,resp.c_str(), resp.size(), 0);
// 	}
// 	else
// 	{
// 		Client *cli = GetClient(fd);
// 		if(cli)
// 		{
// 			// ":" + oldNick + " NICK " + nick + "\r\n"
// 			std::string oldNick = cli->GetNickName();
// 			if(!oldNick.empty())
// 			{
// 				std::string resp = ":" + oldNick + " NICK " + nickname + "\r\n";
// 				std::cout << resp;
// 				send(fd, resp.c_str(), resp.size(), 0);
// 			}
// 			cli->SetNickname(nickname);
// 		}
// 		else
// 		{
// 			std::string resp = ": 541 " + nickname + " :You have not registered\r\n";
// 			send(fd,resp.c_str(), resp.size(), 0);
// 			close(fd);
// 			RemoveFds(fd);
// 			RemoveClient(fd);
// 		}
// 	}
// }



// bool Server::is_clientExist(int fd)
// {
// 	for (size_t i = 0; i < this->clients.size(); i++)
// 	{
// 		if (this->clients[i].GetFd() == fd)
// 			return true;
// 	}
// 	return false;
// }
// bool Server::nickNameInUse(std::string& nickname)
// {
// 	for (size_t i = 0; i < this->clients.size(); i++)
// 	{
// 		if (this->clients[i].GetNickName() == nickname)
// 			return true;
// 	}
// 	return false;
// }

// void Server::client_authen(int fd, std::string& cmd)
// {
// 	Client cli;
// 	std::vector<std::string> splited_cmd = split_cmd(cmd);
// 	if(splited_cmd.size() < 2)
// 	{
// 		std::string resp = ":localhost 461 user :Not enough parameters\r\n";
// 		send(fd, resp.c_str(), resp.size(), 0);
// 	}
// 	else if(!is_clientExist(fd))
// 	{
// 		std::string pass = splited_cmd[1];
// 		if(pass == password)
// 		{
// 			cli.SetFd(fd);
// 			clients.push_back(cli);
// 		}
// 		else
// 		{
// 			std::string resp = ":localhost 464 user :Password incorrect\r\n";
// 			send(fd, resp.c_str(), resp.size(),0);
// 			close(fd);
// 			RemoveFds(fd);
// 			RemoveClient(fd);
// 		}
// 	}
// 	else
// 	{
// 		std::string resp = ":localhost 642 user :You may not reregister\r\n";
// 		send(fd, resp.c_str(), resp.size(),0);

// 	}
// }

void Server::senderror(int code, std::string clientname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << msg;
	std::string resp = ss.str();
	send(fd, resp.c_str(), resp.size(),0);
}

void Server::senderror(int code, std::string clientname, std::string channelname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << " " << channelname << msg;
	std::string resp = ss.str();
	send(fd, resp.c_str(), resp.size(),0);
}

//------------------------TOPIC--------------------------------------------------------------

std::string Server::getnamechannel(std::string &name)
{
	if (!name.empty() && name[0] == '#')
		return (name.substr(1));
	return name;
}
Channel *Server::GetChannel(std::string name)
{
	for (size_t i = 0; i < this->channels.size(); i++){
		if (this->channels[i].GetName() == name)
			return &channels[i];
	}
	return NULL;
}
/*

// bool Server::checkifadmin(int &fd)
// {
// 	for(size_t i=0; i < channels.size(); i++)
// 		if (channels[i].get_admin(fd))
// 			return true;
// 	return false;
// }

// bool Server::checkifchannelexist(std::string &namechannel)
// {
// 	for (size_t i = 0; i < channels.size(); i++)
// 	{
// 		if (namechannel == (channels[i].GetName()))
// 			return true;
// 	}
// 		return false;
// }

// //---------------------------KHBOUYCH-------------------------------------
// std::string Server::tTopic()
// {
// 	std::time_t current = std::time(NULL);
// 	std::stringstream res;

// 	res << current;
// 	return res.str();
// }
// void Server::Topic(std::string &cmd, int &fd)
// {
// 	std::vector<std::string> scmd = split_cmd(cmd);
// 	std::string nmch = getnamechannel(scmd[1]);
// 	Channel *ch = NULL;
// 	int chexist = 0;
// 	for (size_t i = 0; i < channels.size(); i++)
// 	{
// 		if (nmch == channels[i].GetName())
// 		{
// 			ch = &channels[i];
// 			chexist = 1;
// 			break;
// 		}
// 		chexist = 0;
// 	}
// 	if (chexist)
// 	{
// 		Client *admin = ch->get_admin(fd);
// 		Client *client = ch->get_client(fd);
// 		if (scmd.size() == 2)
// 		{
// 			if (admin)
// 			{
// 				std::string respons = ":localhost 332 " + admin->GetUserName() + " " + scmd[1] + " :" + ch->GetTopicName() + "\r\n";
// 				ch->sendTo_all(scmd[1],fd);
// 				send(fd, respons.c_str(), respons.size(),0);
// 				std::string rep1 = ":localhost 333 " + admin->GetNickName() + " " + scmd[1] + " " + admin->GetNickName() + " " + tTopic() + "\r\n";
// 				ch->sendTo_all(scmd[1],fd);
// 				send(fd, rep1.c_str(), rep1.size(),0);
// 			}
// 			else if (client)
// 			{
// 				std::string respons = ":localhost 332 " + client->GetUserName() + " " + scmd[1] + " :" + ch->GetTopicName() + "\r\n";
// 				ch->sendTo_all(scmd[1],fd);
// 				send(fd, respons.c_str(), respons.size(),0);
// 				std::string rep1 = ":localhost 333 " + client->GetNickName() + " " + scmd[1] + " " + client->GetNickName() + " " + tTopic() + "\r\n";
// 				ch->sendTo_all(scmd[1],fd);
// 				send(fd, rep1.c_str(), rep1.size(),0);
// 			}
// 		}

// 		if (scmd.size() == 3)
// 		{
// 			if (ch->Gettopic_restriction() && client)
// 			{
// 				std::string respons =  ":localhost 482 "+client->GetNickName() + scmd[1] +" : You're Not a channel operator";
// 				send(fd, respons.c_str(), respons.size(),0);
// 			}
// 			else if (ch->Gettopic_restriction() && admin)
// 			{
// 				std::string restopic ;
// 				for (size_t i = 2; i < scmd.size(); i++)
// 					restopic += scmd[i] + " ";
// 				ch->SetTopicName(restopic);
// 				std::string respons= ":" + admin->GetNickName() + "!" + admin->GetUserName() + "@localhost TOPIC #" + nmch + " :" + ch->GetTopicName() + "\r\n";
// 				ch->sendTo_all(scmd[1],fd);
// 				send(fd, respons.c_str(), respons.size(),0);
// 			}
// 			else
// 			{
// 				if (admin)
// 				{
// 					std::string restopic;
// 					for (size_t i = 2; i < scmd.size(); i++)
// 						restopic += scmd[i] + " ";
// 					ch->SetTopicName(restopic);
// 					std::string respons= ":" + admin->GetNickName() + "!" + admin->GetUserName() + "@localhost TOPIC #" + nmch + " :" + ch->GetTopicName() + "\r\n";
// 					ch->sendTo_all(scmd[1],fd);
// 					send(fd, respons.c_str(), respons.size(),0);
// 				}
// 				else if (client)
// 				{
// 					std::string restopic ;
// 					for (size_t i = 2; i < scmd.size(); i++)
// 						restopic += scmd[i] + " ";
// 					ch->SetTopicName(restopic);
// 					std::string respons= ":" + client->GetNickName() + "!" + client->GetUserName() + "@localhost TOPIC #" + nmch + " :" + ch->GetTopicName() + "\r\n";
// 					ch->sendTo_all(scmd[1],fd);
// 					send(fd, respons.c_str(), respons.size(),0);
// 				}
// 			}
// 		}
// 	}
// 	else
// 	{
// 		std::string respons = ":localhost 403 " + GetClient(fd)->GetNickName() + " " + nmch + " :No such channel\r\n";
// 		send(fd, respons.c_str(), respons.size(),0);
// 	}
// }

// Client* Server::GetClientbynickname(std::string &nickname, Channel &channel)
// {
// 	(void)channel;
// 	(void)nickname;
// 	return NULL;
// 	// for (size_t i = 0; i < channel.clients.size(); i++)
// 	// {
// 	// 	if (channel.clients[i].GetNickName() == nickname)
// 	// 		return &channel.clients[i];
// 	// }
// }

// void Server::Invite(std::string &cmd, int &fd)
// {
// 	(void)fd;
// 	std::vector<std::string> scmd = split_cmd(cmd);
// 	Channel *ch = GetChannel(scmd[2]);

// 	Client *clt = GetClientbynickname(scmd[1], *ch);

// 	if (clt)
// 	{
// 		std::cout << "--> nickname : " + clt->GetNickName() << std::endl;
// 	}
// 	else
// 	{
// 		puts("---> nop");
// 	}

// }
//---------------------------KHBOUYCH----------------------------------------------------------------
*/
////////////// send

void Server::_sendResponse(std::string response, int fd)
{
	std::cout << "Response:\n" << response;
	if(send(fd, response.c_str(), response.size(), 0) == -1)
		std::cerr << "send() faild" << std::endl;
}
