/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatir <afatir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 09:55:31 by afatir            #+#    #+#             */
/*   Updated: 2024/01/23 18:57:04 by afatir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

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

#include "Client.hpp"
#include "Channel.hpp"
class Client;
class Channel;

class Server
{
private:
	std::vector<Client> clients;
	std::vector<Channel> channels;
	std::vector<struct pollfd> fds;
	int port;
	std::string password;
	int server_fdsocket;
	struct sockaddr_in add;
	struct sockaddr_in cliadd;
	struct pollfd new_cli;
public:
	Server();
	Server(int port, std::string password);
	~Server();
	Server(Server const &src);
	Server &operator=(Server const &src);

	int GetFd();
	int GetPort();
	std::string GetPassword();
	
	void SetFd(int server_fdsocket);
	void SetPort(int port);
	void SetPassword(std::string password);
	
	void AddClient(Client newClient);
	void AddChannel(Channel newChannel);
	void AddFds(pollfd newFd);

	void RemoveClient(int fd);
	void RemoveChannel(std::string name);
	void RemoveFds(int fd);
	//######################
	void                        init(int port, std::string pass);
	void                        set_sever_socket();
	int                         get_sever_socket();
	void                        accept_new_client();
	void                        accept_new_message(int fd, size_t pos);
	std::vector<std::string>    split_cmd(std::string &str);
	void                        client_authen(int fd, std::string& pass, std::vector<struct pollfd> &fds);
	void                        parse_exec_cmd(std::vector<std::string>& cmd, int fd);
};

#endif