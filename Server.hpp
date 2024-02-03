/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatir <afatir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 09:55:31 by afatir            #+#    #+#             */
/*   Updated: 2024/02/03 06:33:37 by afatir           ###   ########.fr       */
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
#include <csignal>
#include <fstream>
#include <ctime>
#include "Client.hpp"
#include "Channel.hpp"
#include "replies.hpp"

class Client;
class Channel;

class Server
{
private:
	static bool Signal;
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
	Client *GetClient(int fd);
	Client *GetClientNick(std::string nickname);
	Channel *GetChannel(std::string name);
	
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
	static void					SignalHandler(int signum);
	void						close_fds();
	void                        init(int port, std::string pass);
	void                        set_sever_socket();
	void                        accept_new_client();
	void                        accept_new_message(int fd);
	std::vector<std::string>    split_cmd(std::string &str);
	void                        client_authen(int fd, std::string& pass, std::vector<struct pollfd> &fds);
	void                        parse_exec_cmd(std::vector<std::string>& cmd, int fd);
	// ########################### JOIN CMD
	void	JOIN(std::string cmd, int fd);
	void	ExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd);
	void	NotExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int fd);
	int		SearchForClients(std::string nickname);
	// ########### CMDS RECIVED
	std::vector<std::string>    split_recivedBuffer(std::string &str);
	void                        client_authen(int fd, std::string& pass);
	void                        parse_exec_cmd(std::string &cmd, int fd);
	std::string 		get_clientList(Channel &channel);
	// void						join_channel(std::vector<std::string>& cmd, int fd);
	// bool						is_channlExist(std::string& channel_name);
	// void                        set_username(std::vector<std::string>& cmd, int fd);
	// Client*					isCliExist(int fd);
	// ########################### BOOL METHODS 
	bool	is_clientExist(int fd);
	bool	nickNameInUse(std::string& nickname);
	bool	is_validNickname(std::string& nickname);
	// ########################### GETTERS & SETTERS 
	void						set_username(std::string& username, int fd);
	void						set_nickname(std::string& nickname, int fd);
	// ########################### CKIK CMD
	void   KICK(std::string cmd, int fd);
	// Channel* GetClientInChannel(Channel chnnel, std::string name);
	// ########################### PART CMD
	void   PART(std::string cmd, int fd);

	bool checkifchannelexist(std::string &namechannel);
	bool checkifadmin(int &fd);
	std::string getnamechannel(std::string &cmd);
	// Channel *GetChannelByName(const std::string &name);

	// ########################### MODE CMD
	void 		mode_command(std::string& cmd, int fd);
	std::string invite_only(Channel *channel, char opera, std::string chain);
	std::string topic_restriction(Channel *channel ,char opera, std::string chain);
	std::string password_mode(std::vector<std::string> splited, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string& arguments);
	std::string operator_privilege(std::vector<std::string> splited, Channel *channel, size_t& pos, int fd, char opera, std::string chain, std::string& arguments);
	std::string channel_limit(std::vector<std::string> splited, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string& arguments);
	bool		isvalid_limit(std::string& limit);
	std::string mode_toAppend(std::string chain, char opera, char mode);
	void 		_sendResponse(std::string response, int fd);
	// ########################### PRIVMSG CMD
	void   PRIVMSG(std::string cmd, int fd);
	void CheckForChannels_Clients(std::vector<std::string> &tmp, int fd);
	//--------KHBOUYCh-------------
	std::string tTopic();
	void Topic(std::string &cmd, int &fd);
	void Invite(std::string &cmd, int &fd);
	Client* GetClientbynickname(std::string &nickname,Channel &channel);
	//--------KHBOUYCh-------------
};

#endif