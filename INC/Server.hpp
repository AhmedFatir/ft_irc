#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
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
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include "Client.hpp"
#include "Channel.hpp"
#include "replies.hpp"


#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

class Client;
class Channel;

class Server
{
private:
	int port;
	int server_fdsocket;
	static bool Signal;
	std::string password;
	std::vector<Client> clients;
	std::vector<Channel> channels;
	std::vector<struct pollfd> fds;
	struct sockaddr_in add;
	struct sockaddr_in cliadd;
	struct pollfd new_cli;
public:
	Server();
	~Server();
	Server(Server const &src);
	Server &operator=(Server const &src);
	//---------------//Getters
	static bool isBotfull;
	int GetFd();
	int GetPort();
	std::string GetPassword();
	Client *GetClient(int fd);
	Client *GetClientNick(std::string nickname);
	Channel *GetChannel(std::string name);
	//---------------//Setters
	void SetFd(int server_fdsocket);
	void SetPort(int port);
	void SetPassword(std::string password);
	void AddClient(Client newClient);
	void AddChannel(Channel newChannel);
	void AddFds(pollfd newFd);
	void set_username(std::string& username, int fd);
	void set_nickname(std::string cmd, int fd);
	//---------------//Remove Methods
	void RemoveClient(int fd);
	void RemoveChannel(std::string name);
	void RemoveFds(int fd);
	void RmChannels(int fd);
	//---------------//Send Methods
	void senderror(int code, std::string clientname, int fd, std::string msg);
	void senderror(int code, std::string clientname, std::string channelname, int fd, std::string msg);
	void 		_sendResponse(std::string response, int fd);
	//---------------//Close and Signal Methods
	static void SignalHandler(int signum);
	void close_fds();
	//---------------//Server Methods
	void init(int port, std::string pass);
	void accept_new_client();
	void set_sever_socket();
	void reciveNewData(int fd);
	//---------------//Parsing Methods
	std::vector<std::string> split_recivedBuffer(std::string str);
	std::vector<std::string> split_cmd(std::string &str);
	void parse_exec_cmd(std::string &cmd, int fd);
	//---------------//Authentification Methods
	bool BypassForBot(std::string cmd, int fd);
	bool notregistered(int fd);
	bool nickNameInUse(std::string& nickname);
	bool is_validNickname(std::string& nickname);
	void client_authen(int fd, std::string pass);
	//---------------------------//JOIN CMD
	void	JOIN(std::string cmd, int fd);
	int		SplitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd);
	void	ExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd);
	void	NotExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int fd);
	int		SearchForClients(std::string nickname);
	//---------------------------//PART CMD
	void	PART(std::string cmd, int fd);
	int		SplitCmdPart(std::string cmd, std::vector<std::string> &tmp, std::string &reason, int fd);
	//---------------------------//CKIK CMD
	void	KICK(std::string cmd, int fd);
	std::string SplitCmdKick(std::string cmd, std::vector<std::string> &tmp, std::string &user, int fd);
	//---------------------------//PRIVMSG CMD
	void	PRIVMSG(std::string cmd, int fd);
	void	CheckForChannels_Clients(std::vector<std::string> &tmp, int fd);
	//---------------------------//QUITE CMD
	void	QUIT(std::string cmd, int fd);
	//---------------------------//MODE CMD
	void 		mode_command(std::string& cmd, int fd);
	std::string invite_only(Channel *channel, char opera, std::string chain);
	std::string topic_restriction(Channel *channel ,char opera, std::string chain);
	std::string password_mode(std::vector<std::string> splited, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string& arguments);
	std::string operator_privilege(std::vector<std::string> splited, Channel *channel, size_t& pos, int fd, char opera, std::string chain, std::string& arguments);
	std::string channel_limit(std::vector<std::string> splited, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string& arguments);
	bool		isvalid_limit(std::string& limit);
	std::string mode_toAppend(std::string chain, char opera, char mode);
	std::vector<std::string> splitParams(std::string params);
	void getCmdArgs(std::string cmd,std::string& name, std::string& modeset ,std::string &params);
	//---------------------------//TOPIC CMD
	std::string tTopic();
	void Topic(std::string &cmd, int &fd);
	void Invite(std::string &cmd, int &fd);
	std::string gettopic(std::string& input);
	int getpos(std::string &cmd);
};

#endif