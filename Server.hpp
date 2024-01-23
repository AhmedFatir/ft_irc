/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatir <afatir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 09:55:31 by afatir            #+#    #+#             */
/*   Updated: 2024/01/23 04:10:01 by afatir           ###   ########.fr       */
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
	std::vector<pollfd> fds;
	std::string password;
	int port;
	int fd;
public:
	Server();
	Server(int port, std::string password);
	~Server();
	Server(Server const &src);
	Server &operator=(Server const &src);

	int GetFd();
	int GetPort();
	std::string GetPassword();
	
	void SetFd(int fd);
	void SetPort(int port);
	void SetPassword(std::string password);
	
	void AddClient(Client newClient);
	void AddChannel(Channel newChannel);
	void AddFds(pollfd newFd);

	void RemoveClient(int fd);
	void RemoveChannel(std::string name);
	void RemoveFds(int fd);
	int InitServer();
};

#endif