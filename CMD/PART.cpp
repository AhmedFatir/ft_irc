#include "../Server.hpp"

std::string Server::SplitCmdPart(std::string cmd, std::vector<std::string> &tmp, int fd)
{
	std::istringstream stm(cmd);
	std::string reason;
	while(stm >> cmd)
		tmp.push_back(cmd);
	tmp.erase(tmp.begin());
	for (size_t i = 1; i < tmp.size(); i++){//start from the second string and take the rest of the strings as the reason
			for (size_t j = i; j < tmp.size(); j++)
				{reason += " " + tmp[j];tmp.erase(tmp.begin() + j);j--;}
	}
	std::string str = tmp[0]; std::string str1; tmp.clear();
	for (size_t i = 0; i < str.size(); i++){//split the first string by ',' to get the channels names
		if (str[i] == ',')
			{tmp.push_back(str1); str1.clear();}
		else str1 += str[i];
	}
	tmp.push_back(str1);
	for (size_t i = 0; i < tmp.size(); i++)//erase the empty strings
		{if (tmp[i].empty())tmp.erase(tmp.begin() + i);}
	for (size_t i = 0; i < tmp.size(); i++){// erase the '#' from the channel name and check if the channel valid
			if (*(tmp[i].begin()) == '#')
				tmp[i].erase(tmp[i].begin());
			else
				{senderror(403, GetClient(fd)->GetNickName(), tmp[i], GetClient(fd)->GetFd(), " :No such channel\r\n"); tmp.erase(tmp.begin() + i); i--;}
		}
	if (!reason.empty())// if the reason is not empty add it thr quotes
		{reason.erase(reason.begin());reason = "\"" + reason + "\"";}
	return reason;
}

void Server::PART(std::string cmd, int fd)
{
	if (cmd.size() < 6)// ERR_NEEDMOREPARAMS (461) // if the channel name is empty
		{senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	std::vector<std::string> tmp;
	std::string reason = SplitCmdPart(cmd, tmp, fd);
	for (size_t i = 0; i < tmp.size(); i++){
		bool flag = false;
		for (size_t j = 0; j < this->channels.size(); j++){ // search for the channel
			if (this->channels[j].GetName() == tmp[i]){ // check if the channel exist
				flag = true;
				if (!channels[j].get_client(fd) && !channels[j].get_admin(fd)) // ERR_NOTONCHANNEL (442) // if the client is not in the channel
					{senderror(442, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :You're not on that channel\r\n"); return;}
					std::stringstream ss;
					ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName() << "@" << "localhost" << " PART #" << tmp[i];
					if (!reason.empty())
						ss << " :" << reason << "\r\n";
					else ss << "\r\n";
					send(fd, ss.str().c_str(), ss.str().size(),0);
					std::cout << "		" << ss.str();
					if (channels[j].get_admin(channels[j].GetClientInChannel(GetClient(fd)->GetNickName())->GetFd()))
						channels[j].remove_admin(channels[j].GetClientInChannel(GetClient(fd)->GetNickName())->GetFd());
					else
						channels[j].remove_client(channels[j].GetClientInChannel(GetClient(fd)->GetNickName())->GetFd());
					if (channels[j].GetClientsNumber() == 0)
						channels.erase(channels.begin() + j);
			}
		}
		if (!flag) // ERR_NOSUCHCHANNEL (403) // if the channel doesn't exist
			senderror(403, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :No such channel\r\n");
	}
}