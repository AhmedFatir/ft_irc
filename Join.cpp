#include "Server.hpp"

/*
    JOIN COMMAND
*/

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
	//print the vector
	// for (size_t i = 0; i < tmp.size(); i++)
	// 	std::cout << tmp[i] << std::endl;
	// exit(0);
}

void SplitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd)
{
	std::vector<std::string> tmp;
	SplitCmdJoin(cmd, tmp);
	//print the vector
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
	//print the vector
	// for (size_t i = 0; i < token.size(); i++)
	// 	std::cout << token[i].first << " " << token[i].second << std::endl;
	// exit(0);
}

void Server::senderror(int code, std::string clientname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << msg;
	std::string resp = ss.str();
    _sendResponse(resp, fd);
}

void Server::senderror(int code, std::string clientname, std::string channelname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << " " << channelname << msg;
	std::string resp = ss.str();
    _sendResponse(resp, fd);
}


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
    std::string response = RPL_JOINMSG(cli->getHostname(),std::string("localhost"),token[i].first) + \
        RPL_NAMREPLY(GetClient(fd)->GetNickName(),token[i].first,channels[j].clientChannel_list()) + \
        RPL_ENDOFNAMES(GetClient(fd)->GetNickName(),token[i].first);
    channels[i].sendTo_all(response);
}


void Server::NotExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int fd)
{
	if (SearchForClients(GetClient(fd)->GetNickName()) >= 10)//ERR_TOOMANYCHANNELS (405) // if the client is already in 10 channels
		{senderror(405, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :You have joined too many channels\r\n"); return;}
	Channel newChannel;
	newChannel.SetName(token[i].first);
	if (!token[i].second.empty())
		newChannel.SetPassword(token[i].second);
	newChannel.add_admin(*GetClient(fd));
	newChannel.set_createiontime();
	this->channels.push_back(newChannel);
	// notifiy thet the client joined the channel
    _sendResponse(RPL_JOINMSG(GetClient(fd)->getHostname(),std::string("localhost"),newChannel.GetName()) + \
        RPL_NAMREPLY(GetClient(fd)->GetNickName(),newChannel.GetName(),newChannel.clientChannel_list()) + \
        RPL_ENDOFNAMES(GetClient(fd)->GetNickName(),newChannel.GetName()),fd);
}
void Server::JOIN(std::string cmd, int fd)
{
	if (cmd.size() < 6)// ERR_NEEDMOREPARAMS (461) // if the channel name is empty
		{senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	std::vector<std::pair<std::string, std::string> > token;
	SplitJoin(token, cmd);
	std::cout <<"-------------------------"<< token[0].first << std::endl;
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