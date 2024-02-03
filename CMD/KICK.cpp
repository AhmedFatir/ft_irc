#include "../Server.hpp"

void SplitCmdKick(std::string cmd, std::vector<std::string> &tmp)
{
	std::istringstream stm(cmd);
	std::string str;
	while(stm >> cmd)
		tmp.push_back(cmd);
	tmp.erase(tmp.begin());
	for (size_t i = 2; i < tmp.size(); i++){//take just the first 2 strings and rest of the string is the reason
			for (size_t j = i; j < tmp.size(); j++)
				{str += " " + tmp[j];tmp.erase(tmp.begin() + j);j--;}
	}
	if (!str.empty()) // if the reason is not empty add it
		tmp.push_back(str);
	tmp[0].erase(tmp[0].begin());//erase the '#' from the channel name
}

void	Server::KICK(std::string cmd, int fd)
{
	//ERR_BADCHANMASK (476) // if the channel mask is invalid
	if (!GetClient(fd)) //ERR_NOTREGISTERED (451) // if the client is not registered
		{senderror(451, "", fd, " :You have not registered\r\n"); return;}
	std::vector<std::string> tmp;
	SplitCmdKick(cmd, tmp);
	int flag = 0;
	if (tmp.size() < 2) // check if the client send the channel name and the client to kick
		{senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	for (size_t i = 0; i < this->channels.size(); i++){ // search for the channel
		if (this->channels[i].GetName() == tmp[0]){ // check if the channel exist
			flag = 1;
			if (!channels[i].get_client(fd) && !channels[i].get_admin(fd)) // check if the client is in the channel
				{senderror(442, GetClient(fd)->GetNickName(), tmp[0], GetClient(fd)->GetFd(), " :You're not on that channel\r\n"); return;}
			if(this->channels[i].get_admin(fd)){ // check if the client is admin
				if (channels[i].GetClientInChannel(tmp[1])){ // check if the client to kick is in the channel
					std::stringstream ss;
					ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName() << "@" << "localhost" << " KICK #" << tmp[0] << " " << tmp[1];
					if (tmp.size() == 3)
						ss << " :" << tmp[2] << "\r\n";
					else ss << "\r\n";
					this->channels[i].sendTo_all(ss.str());
					std::cout << "		" << ss.str();
					if (channels[i].get_admin(channels[i].GetClientInChannel(tmp[1])->GetFd()))
						channels[i].remove_admin(channels[i].GetClientInChannel(tmp[1])->GetFd());
					else
						channels[i].remove_client(channels[i].GetClientInChannel(tmp[1])->GetFd());
					if (channels[i].GetClientsNumber() == 0)
						channels.erase(channels.begin() + i);
				}
				else // if the client to kick is not in the channel
					{senderror(441, GetClient(fd)->GetNickName(), tmp[0], GetClient(fd)->GetFd(), " :They aren't on that channel\r\n"); return;}
			}
			else // if the client is not admin
				{senderror(482, GetClient(fd)->GetNickName(), tmp[0], GetClient(fd)->GetFd(), " :You're not channel operator\r\n"); return;}
		}
	}
	if (flag == 0) // if the channel doesn't exist
		senderror(403, GetClient(fd)->GetNickName(), tmp[0], GetClient(fd)->GetFd(), " :No such channel\r\n");
}