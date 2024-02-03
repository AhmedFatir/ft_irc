#include "../Server.hpp"

std::string SplitCmdPart(std::string cmd, std::vector<std::string> &tmp)
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
	for (size_t i = 0; i < tmp.size(); i++)// erase the '#' from the channel name
		{if (*(tmp[i].begin()) == '#' || *(tmp[i].begin()) == '&') tmp[i].erase(tmp[i].begin());}
	return reason;
}

void Server::PART(std::string cmd, int fd)
{
	if (!GetClient(fd)) //ERR_NOTREGISTERED (451) // if the client is not registered
		{senderror(451, "", fd, " :You have not registered\r\n"); return;}
	if (cmd.size() < 6)// ERR_NEEDMOREPARAMS (461) // if the channel name is empty
		{senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	std::vector<std::string> tmp;
	std::string reason = SplitCmdPart(cmd, tmp);
	for (size_t i = 0; i < tmp.size(); i++){
		bool flag = false;
		for (size_t j = 0; j < this->channels.size(); j++){ // search for the channel
			if (this->channels[j].GetName() == tmp[i]){ // check if the channel exist
				flag = true;
				if (!channels[j].get_client(fd) && !channels[j].get_admin(fd)) // ERR_NOTONCHANNEL (442) // if the client is not in the channel
					{senderror(442, GetClient(fd)->GetNickName(), tmp[i], GetClient(fd)->GetFd(), " :You're not on that channel\r\n"); return;}
					std::stringstream ss;
					ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName() << "@" << "localhost" << " PART #" << tmp[i];
					if (!reason.empty())
						ss << " :" << reason << "\r\n";
					else ss << "\r\n";
					std::string resp = ss.str();
					send(fd, resp.c_str(), resp.size(),0);
					std::cout << "		" << resp;
					if (channels[j].get_admin(channels[j].GetClientInChannel(GetClient(fd)->GetNickName())->GetFd()))
						channels[j].remove_admin(channels[j].GetClientInChannel(GetClient(fd)->GetNickName())->GetFd());
					else
						channels[j].remove_client(channels[j].GetClientInChannel(GetClient(fd)->GetNickName())->GetFd());
			}
		}
		if (!flag) // ERR_NOSUCHCHANNEL (403) // if the channel doesn't exist
			senderror(403, GetClient(fd)->GetNickName(), tmp[i], GetClient(fd)->GetFd(), " :No such channel\r\n");
	}
}