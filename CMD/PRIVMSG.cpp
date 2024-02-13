#include "../Server.hpp"

std::string SplitCmdPrivmsg(std::string cmd, std::vector<std::string> &tmp)
{
	std::istringstream stm(cmd);
	std::string str;
	while(stm >> cmd)
		tmp.push_back(cmd);
	tmp.erase(tmp.begin());
	for (size_t i = 1; i < tmp.size(); i++){//start from the second string and take the rest of the strings as the message
			for (size_t j = i; j < tmp.size(); j++)
				{str += " " + tmp[j];tmp.erase(tmp.begin() + j);j--;}
	}
	std::string str1 = tmp[0]; std::string str2; tmp.clear();
	for (size_t i = 0; i < str1.size(); i++){//split the first string by ',' to get the channels names
		if (str1[i] == ',')
			{tmp.push_back(str2); str2.clear();}
		else str2 += str1[i];
	}
	tmp.push_back(str2);
	for (size_t i = 0; i < tmp.size(); i++)//erase the empty strings
		{if (tmp[i].empty())tmp.erase(tmp.begin() + i--);}
	str.erase(0, 1);
	if (str[0] == ':') str.erase(str.begin());
	else //shrink to the first space
		{for (size_t i = 0; i < str.size(); i++){if (str[i] == ' '){str = str.substr(0, i);break;}}}
	return  str;
	
}

void	Server::CheckForChannels_Clients(std::vector<std::string> &tmp, int fd)
{
	for(size_t i = 0; i < tmp.size(); i++){
		if (tmp[i][0] == '#'){
			tmp[i].erase(tmp[i].begin());
			if(!GetChannel(tmp[i]))//ERR_NOSUCHNICK (401) // if the channel doesn't exist
				{senderror(401, "#" + tmp[i], GetClient(fd)->GetFd(), " :No such nick/channel\r\n"); tmp.erase(tmp.begin() + i); i--;}
			else if (!GetChannel(tmp[i])->GetClientInChannel(GetClient(fd)->GetNickName())) //ERR_CANNOTSENDTOCHAN (404) // if the client is not in the channel
				{senderror(404, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :Cannot send to channel\r\n"); tmp.erase(tmp.begin() + i); i--;}
			else tmp[i] = "#" + tmp[i];
		}
		else{
			if (!GetClientNick(tmp[i]))//ERR_NOSUCHNICK (401) // if the client doesn't exist
				{senderror(401, tmp[i], GetClient(fd)->GetFd(), " :No such nick/channel\r\n"); tmp.erase(tmp.begin() + i); i--;}
		}
	}
}

void	Server::PRIVMSG(std::string cmd, int fd)
{
	if (cmd.size() < 9)//ERR_NORECIPIENT (411) // if the client doesn't specify the recipient
		{senderror(411, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :No recipient given (PRIVMSG)\r\n"); return;}
	std::vector<std::string> tmp;
	std::string message = SplitCmdPrivmsg(cmd, tmp);
	if (message.empty())//ERR_NOTEXTTOSEND (412) // if the client doesn't specify the message
		{senderror(412, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :No text to send\r\n"); return;}
	if (tmp.size() > 10) //ERR_TOOMANYTARGETS (407) // if the client send the message to more than 10 clients
		{senderror(407, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Too many recipients\r\n"); return;}
	CheckForChannels_Clients(tmp, fd); // check if the channels and clients exist
	for (size_t i = 0; i < tmp.size(); i++){// send the message to the clients and channels
		if (tmp[i][0] == '#'){
			tmp[i].erase(tmp[i].begin());
			std::string resp = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost PRIVMSG #" + tmp[i] + " :" + message + "\r\n";
			GetChannel(tmp[i])->sendTo_all(resp, fd);
		}
		else{
			std::string resp = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost PRIVMSG " + tmp[i] + " :" + message + "\r\n";
			_sendResponse(resp, GetClientNick(tmp[i])->GetFd());
		}
	}
}
