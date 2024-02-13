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
void	Server::RmChannels(int fd)
{
	for (size_t i = 0; i < this->channels.size(); i++){
		int flag = 0;
		if (channels[i].get_client(fd))
			{channels[i].remove_client(fd); flag = 1;}
		else if (channels[i].get_admin(fd))
			{channels[i].remove_admin(fd); flag = 1;}
		if (channels[i].GetClientsNumber() == 0)
			{channels.erase(channels.begin() + i); i--; continue;}
		if (flag){
			std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost QUIT Quit\r\n";
			channels[i].sendTo_all(rpl);
			std::cout << "QUIT: " << GetClient(fd)->GetNickName() << " has left the channel #" << channels[i].GetName() << std::endl;
		}
	}
}

void	Server::close_fds()
{
	for(size_t i = 0; i < clients.size(); i++)
	{
		// std::cout << "close index: " << i << " /NumFd: "<< clients[i].GetFd() << std::endl;
		close(clients[i].GetFd());
	}
	// std::cout << "close the server's Fd: " << server_fdsocket << std::endl;
	close(server_fdsocket);
}

bool Server::Signal = false;

void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "SignalHandler" << std::endl;
	Server::Signal = true;
}

void Server::addNewClient(int fd)
{
	Client client;

	client.SetFd(fd);
	clients.push_back(client);
}

void Server::init(int port, std::string pass)
{
	this->password = pass;
	this->port = port;
	this->set_sever_socket();

	std::cout << "Waiting to accept a connection...\n";
	while (Server::Signal == false)
	{
		// if(poll(&fds[0],fds.size(),-1) == -1)
		if(poll(fds.data(),fds.size(),-1) == -1)
			std::cout << "poll() faild or signal recived" << std::endl;
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
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
	fcntl(server_fdsocket, F_SETFL, O_NONBLOCK);
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
	Client cli;
	memset(&cliadd, 0, sizeof(cliadd));
	socklen_t len = sizeof(cliadd);
	int incofd = accept(server_fdsocket, (sockaddr *)&(cliadd), &len);
	if (incofd == -1)
		throw(std::runtime_error("faccept() failed"));
	fcntl(incofd, F_SETFL, O_NONBLOCK);
	new_cli.fd = incofd;
	new_cli.events = POLLIN;
	new_cli.revents = 0;
	cli.SetFd(incofd);
	cli.setIpAdd(inet_ntoa((cliadd.sin_addr)));
	clients.push_back(cli);
	fds.push_back(new_cli);
}

std::vector<std::string> Server::split_recivedBuffer(std::string str)
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
	Client *cli = GetClient(fd);
	ssize_t bytes = recv(fd, buff, sizeof(buff), 0);
	std::vector<std::string> cmd;
	if(bytes <= 0)
	{
		std::cout << "clinet: " << fd << " disconnected" << std::endl;
		RmChannels(fd);
		RemoveClient(fd);
		RemoveFds(fd);
		close(fd);
	}
	else
	{
		if (bytes < 1024) buff[bytes] = '\0';
		else buff[1023] = '\0';
		cli->setBuffer(buff);
		if(cli->getBuffer().find_first_of("\r\n") == std::string::npos)
			return;
		if(cli->getBuffer() != "PONG localhost\r\n")
		{
			std::cout << "recived" <<" [" << cli->GetFd() << "]: " << cli->getBuffer();
			cmd = split_recivedBuffer(cli->getBuffer());
			for(size_t i = 0; i < cmd.size(); i++)
				this->parse_exec_cmd(cmd[i], fd);
			if(GetClient(fd))
				GetClient(fd)->clearBuffer();
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

bool Server::notregistered(int fd)
{
	if (!GetClient(fd) || GetClient(fd)->GetNickName().empty() || GetClient(fd)->GetUserName().empty())
		return false;
	return true;
}

void Server::StartBot(std::string cmd, int fd)
{
	std::string botmsg;
	std::cout << " ===> Recived Msg For Bot :" << cmd;
	if (!GetClientNick("bot"))
		{senderror(401, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :bot not found\r\n"); return;}
	botmsg = GetClient(fd)->GetNickName() + " " + cmd + "\r\n";
	if (send(GetClientNick("bot")->GetFd(), botmsg.c_str(), botmsg.size(), 0) == -1)
		std::cerr << "send() faild" << std::endl;
}

void Server::parse_exec_cmd(std::string &cmd, int fd)
{
	if(cmd.empty())
		return ;
	std::vector<std::string> splited_cmd = split_cmd(cmd);
    if(splited_cmd.size() && (splited_cmd[0] == "PASS" || splited_cmd[0] == "pass"))
        client_authen(fd, cmd);
	else if (splited_cmd.size() && (splited_cmd[0] == "NICK" || splited_cmd[0] == "nick"))
		set_nickname(cmd,fd);
	else if(splited_cmd.size() && (splited_cmd[0] == "USER" || splited_cmd[0] == "user"))
		set_username(cmd, fd);
	else if (splited_cmd.size() && (splited_cmd[0] == "QUIT" || splited_cmd[0] == "quit"))
		QUIT(cmd,fd);
	else if(notregistered(fd))
	{
		if (splited_cmd.size() && (splited_cmd[0] == "KICK" || splited_cmd[0] == "kick"))
			KICK(cmd, fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "JOIN" || splited_cmd[0] == "join"))
			JOIN(cmd, fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "TOPIC" || splited_cmd[0] == "topic"))
			Topic(cmd, fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "MODE" || splited_cmd[0] == "mode"))
			mode_command(cmd, fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "PART" || splited_cmd[0] == "part"))
			PART(cmd, fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "PRIVMSG" || splited_cmd[0] == "privmsg"))
			PRIVMSG(cmd, fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "INVITE" || splited_cmd[0] == "invite"))
			Invite(cmd,fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "PLAY" || splited_cmd[0] == "AGE" || splited_cmd[0] == "NOKTA" \
			|| splited_cmd[0] == "play" || splited_cmd[0] == "age" || splited_cmd[0] == "nokta"))
			StartBot(cmd, fd);
		else if (splited_cmd.size())
			_sendResponse(ERR_CMDNOTFOUND(GetClient(fd)->GetNickName(),splited_cmd[0]),fd);

	}
	else if (!notregistered(fd))
		_sendResponse(ERR_NOTREGISTERED(std::string("nickname")),fd);

}

void Server::senderror(int code, std::string clientname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << msg;
	std::string resp = ss.str();
	if(send(fd, resp.c_str(), resp.size(),0) == -1)
		std::cerr << "send() faild" << std::endl;
}

void Server::senderror(int code, std::string clientname, std::string channelname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << " " << channelname << msg;
	std::string resp = ss.str();
	if(send(fd, resp.c_str(), resp.size(),0) == -1)
		std::cerr << "send() faild" << std::endl;
}

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

void Server::_sendResponse(std::string response, int fd)
{
	// std::cout << "Response:\n" << response;
	if(send(fd, response.c_str(), response.size(), 0) == -1)
		std::cerr << "Response send() faild" << std::endl;
}
