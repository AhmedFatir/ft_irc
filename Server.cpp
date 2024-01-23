/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatir <afatir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 10:06:56 by afatir            #+#    #+#             */
/*   Updated: 2024/01/23 04:24:57 by afatir           ###   ########.fr       */
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
		this->fd = src.fd;
	}
	return *this;
}
void Server::SetFd(int fd){this->fd = fd;}
void Server::SetPort(int port){this->port = port;}
void Server::SetPassword(std::string password){this->password = password;}
int Server::GetFd(){return this->fd;}
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
//##############################################################################################
std::string recive(int fd, char *buffer)
{
	bzero(buffer, BUFSIZ);
	recv(fd, buffer, BUFSIZ, 0);
	return (std::string)buffer;
}

int create_server_socket(int port)
{
	struct sockaddr_in sa;
	int socket_fd;

	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sa.sin_port = htons(port);

	socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
	if (socket_fd == -1)
		throw std::runtime_error("[Server] Socket error: " + std::string(strerror(errno)));
	std::cout << "[Server] Created server socket fd: " << socket_fd << std::endl;

	if (bind(socket_fd, reinterpret_cast<struct sockaddr*>(&sa), sizeof(sa)) != 0)
		throw std::runtime_error("[Server] Bind error: " + std::string(strerror(errno)));
	std::cout << "[Server] Bound socket to localhost port " << port << std::endl;
	return socket_fd;
}

Client NewClient(int fd, std::string password)
{
	std::string nickname, username;
	char buffer[BUFSIZ];
	send(fd, "Enter server password: ", 23, 0);
	if (recive(fd, buffer) != (password+"\n")){
		send(fd, "Wrong password. Closing connection.\n", 36, 0);
		close(fd);
		return Client();
	}
	send(fd, "Enter your nickname: ", 21, 0);
	nickname = recive(fd, buffer);
	send(fd, "Enter your username: ", 21, 0);
	username = recive(fd, buffer);
	Client newClient(nickname, username, fd);
	return newClient;
}

void accept_new_connection(Server &ser)
{
	int fd;
	char msg_to_send[BUFSIZ];

	fd = accept(ser.GetFd(), NULL, NULL);
	if (fd == -1)
		{std::cerr << "[Server] Accept error: " << strerror(errno) << std::endl; return;}
	Client newClient = NewClient(fd, ser.GetPassword());
	if (newClient.GetFd() == -1)
		return;
	ser.AddClient(newClient);
	pollfd client_fd;
	client_fd.fd = fd;
	client_fd.events = POLLIN;
	ser.AddFds(client_fd);
	std::cout << "[Server] Accepted new connection on client socket " << fd << "." << std::endl;
	std::memset(&msg_to_send, '\0', sizeof(msg_to_send));
	std::stringstream ss;
	ss << "Welcome. You are client fd [" << fd << "]\n";
	std::strcpy(msg_to_send, ss.str().c_str());
	if (send(fd, msg_to_send, std::strlen(msg_to_send), 0) == -1)
		std::cerr << "[Server] Send error to client " << fd << ": " << strerror(errno) << std::endl;
}

void read_data_from_socket(int i, std::vector<pollfd>& fds, int server_socket)
{
	char buffer[BUFSIZ];
	char msg_to_send[BUFSIZ];
	int bytes_read;
	int sender_fd;

	sender_fd = fds[i].fd;
	bzero(buffer, BUFSIZ);
	bytes_read = recv(sender_fd, buffer, BUFSIZ, 0);
	if (!std::strcmp(buffer, "exit\n")){
		std::cout << "[" << sender_fd << "] Client socket closed connection." << std::endl;
		close(sender_fd);
		fds.erase(fds.begin() + i);
		return;
	}
	if (bytes_read <= 0)
	{
		if (bytes_read == 0)
			std::cout << "[" << sender_fd << "] Client socket closed connection." << std::endl;
		else
			std::cerr << "[Server] Recv error: " << strerror(errno) << std::endl;
		close(sender_fd);
		fds.erase(fds.begin() + i);
	}
	else
	{
		std::cout << "[" << sender_fd << "] Got message: " << buffer;

		std::memset(&msg_to_send, '\0', sizeof msg_to_send);
		std::stringstream ss;
		ss << "[" << sender_fd << "] says: " << buffer;
		std::strcpy(msg_to_send, ss.str().c_str());
		for (size_t j = 0; j < fds.size(); j++){
			if (fds[j].fd != server_socket && fds[j].fd != sender_fd){
				if (send(fds[j].fd, msg_to_send, std::strlen(msg_to_send), 0) == -1)
					std::cerr << "[Server] Send error to client fd " << fds[j].fd << ": " << strerror(errno) << std::endl;
			}
		}
	}
}

int Server::InitServer()
{
	int status;

	this->SetFd(create_server_socket(this->GetPort()));
	if (this->GetFd() == -1)
		throw std::runtime_error("[Server] Failed to create server socket.");
	std::cout << "[Server] Listening on port " << this->GetPort() << std::endl;
	status = listen(this->GetFd(), SOMAXCONN);
	if (status != 0)
		throw std::runtime_error("[Server] Listen error: " + std::string(strerror(errno)));
	pollfd server_fd;
	server_fd.fd = this->GetFd();
	server_fd.events = POLLIN;
	this->AddFds(server_fd);
	while (true)
	{
		status = poll(this->fds.data(), this->fds.size(), -1);
		if (status == -1)
			throw std::runtime_error("[Server] Poll error: " + std::string(strerror(errno)));
		else if (status == 0)
			continue;
		for (size_t i = 0; i < this->fds.size(); i++){
			if ((this->fds[i].revents & POLLIN) != 1)
				continue;
			if (this->fds[i].fd == this->fd)
				accept_new_connection(*this);
			else
				read_data_from_socket(i, this->fds, this->fd);
		}
		// if (check_exit_command(this->GetFd()))
        //     break;
	}
	for (size_t i = 0; i < this->fds.size(); i++)
        close(this->fds[i].fd);
	close(this->fd);
	return 0;
}
