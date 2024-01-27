#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <csignal>

class Client
{
private:
	std::string nick;
	std::string user;
	int fd;
public:
	Client(std::string nick, std::string user, int fd) : nick(nick), user(user), fd(fd) {}
	std::string GetNick() {return this->nick;}
	std::string GetUser() {return this->user;}
	int GetFd() {return this->fd;}
	void SetNick(std::string nick) {this->nick = nick;}
	void SetUser(std::string user) {this->user = user;}
	void SetFd(int fd) {this->fd = fd;}
};

class Server
{
private:
	int fd;
	int port;
	std::vector<pollfd> fds;
	std::vector<Client> clients;
public:
	Server(int port) : port(port) {}
	int InitServer();
	int GetFd() {return this->fd;}
	int GetPort() {return this->port;}
	void SetFd(int fd) {this->fd = fd;}
	void AddFds(pollfd fd) {this->fds.push_back(fd);}
	void AddClient(Client client) {this->clients.push_back(client);}
};

int create_server_socket(int port)
{
	struct sockaddr_in sa;
	int socket_fd;
	int status;

	std::memset(&sa, 0, sizeof sa);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sa.sin_port = htons(port);

	socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
	if (socket_fd == -1)
		{std::cerr << "[Server] Socket error: " << strerror(errno) << std::endl; return -1;}
	std::cout << "[Server] Created server socket fd: " << socket_fd << std::endl;

	status = bind(socket_fd, reinterpret_cast<struct sockaddr*>(&sa), sizeof(sa));
	if (status != 0)
		throw std::runtime_error("[Server] Bind error: " + std::string(strerror(errno)));
	std::cout << "[Server] Bound socket to localhost port " << port << std::endl;

	return socket_fd;
}

void accept_new_connection(Server &ser)
{
	int fd;
	char msg_to_send[BUFSIZ];
	int status;

	fd = accept(ser.GetFd(), NULL, NULL);
	if (fd == -1)
		{std::cerr << "[Server] Accept error: " << strerror(errno) << std::endl; return;}
	Client newClient("nick", "user", fd);
	ser.AddClient(newClient);

	pollfd client_fd;
	client_fd.fd = fd;
	client_fd.events = POLLIN;
	ser.AddFds(client_fd);

	std::cout << "[Server] Accepted new connection on client socket " << fd << "." << std::endl;

	std::memset(&msg_to_send, '\0', sizeof msg_to_send);
	std::stringstream ss;
	ss << ":localhost 001 user :Welcome to the IRC server!\r\n";
	std::strcpy(msg_to_send, ss.str().c_str());
	
	
	status = send(fd, msg_to_send, std::strlen(msg_to_send), 0);
	if (status == -1)
		std::cerr << "[Server] Send error to client " << fd << ": " << strerror(errno) << std::endl;
}

void JOIN(int fd, char *buffer)
{
	std::string host = "localhost";
    std::string chnam = "#cc";
    std::string nickname = "aa";
    std::string username = "bb";
    std::string clis = "@" + nickname;

    std::cout << std::endl;
    std::cout << "host " << host << std::endl;
    std::cout << "chnam " << chnam << std::endl;
    std::cout << "nickname " << nickname << std::endl;
    std::cout << "username " << username << std::endl;
    std::cout << "clis " << clis << std::endl;
    // ":" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"
    std::cout << std::endl;

    std::string respo = ":" + nickname + "!~" + username + "@localhost JOIN " + chnam + "\n";


    std::string resp2 = ":localhost 353 " + nickname + " @ " + chnam + " :@" + nickname + "\n";

    // // ":" + hostname + " 366 " + nick + " " + channelname + " :END of /NAMES list\r\n"
    std::string resp3 = ":localhost 366 " + nickname + " " + chnam + " :End of /NAMES list.\n";

    std::cout << respo; // join message 
    std::cout << resp2; // RPL_NAMREPLY (353) Messages:
    std::cout << resp3; // RPL_ENDOFNAMES (366) Message:

    send(fd, respo.c_str(), respo.size(),0);
    send(fd, resp2.c_str(), resp2.size(),0);
    send(fd, resp3.c_str(), resp3.size(),0);
}

void read_data_from_socket(int i, std::vector<pollfd>& fds, int server_socket)
{
	char buffer[BUFSIZ];
	char msg_to_send[BUFSIZ];
	int bytes_read;
	int status;
	int sender_fd;

	sender_fd = fds[i].fd;
	std::memset(&buffer, '\0', sizeof buffer);
	bytes_read = recv(sender_fd, buffer, BUFSIZ, 0);
	if (!std::strncmp(buffer, "JOIN", 4))
		JOIN(sender_fd, buffer);
	if (!std::strcmp(buffer, "exit\n"))
	{
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
		for (size_t j = 0; j < fds.size(); j++)
		{ // Loop on fds
			if (fds[j].fd != server_socket && fds[j].fd != sender_fd){
				status = send(fds[j].fd, msg_to_send, std::strlen(msg_to_send), 0);
				if (status == -1)
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
		status = poll(this->fds.data(), this->fds.size(), 2000);
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
	}
	close(this->fd);
	return 0;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
		return 1;
	}
	Server server(std::stoi(argv[1]));
	try
	{
		server.InitServer();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}