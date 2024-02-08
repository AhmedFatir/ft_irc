#include "../Server.hpp"

std::string Server::SplitCmdKick(std::string cmd, std::vector<std::string> &tmp, std::string &user, int fd)
{
	std::istringstream stm(cmd);
	std::string reason;
	while(stm >> cmd)
		tmp.push_back(cmd);
	tmp.erase(tmp.begin());
	for (size_t i = 2; i < tmp.size(); i++){//start from the second string and take the rest of the strings as the reason
			for (size_t j = i; j < tmp.size(); j++)
				{reason += " " + tmp[j];tmp.erase(tmp.begin() + j);j--;}
	}
	if (tmp.size() < 2) // check if the client send the channel name and the client to kick
		return std::string("");
	std::string str = tmp[0]; std::string str1;
	user = tmp[1]; tmp.clear();
	for (size_t i = 0; i < str.size(); i++){//split the first string by ',' to get the channels names
		if (str[i] == ',')
			{tmp.push_back(str1); str1.clear();}
		else str1 += str[i];
	}
	tmp.push_back(str1);
	for (size_t i = 0; i < tmp.size(); i++)//erase the empty strings
		{if (tmp[i].empty())tmp.erase(tmp.begin() + i--);}
	reason.erase(reason.begin());
	if (reason[0] == ':') reason.erase(reason.begin());
	else //shrink to the first space
		{for (size_t i = 0; i < reason.size(); i++){if (reason[i] == ' '){reason = reason.substr(0, i);break;}}}
	for (size_t i = 0; i < tmp.size(); i++){// erase the '#' from the channel name and check if the channel valid
			if (*(tmp[i].begin()) == '#')
				tmp[i].erase(tmp[i].begin());
			else
				{senderror(403, GetClient(fd)->GetNickName(), tmp[i], GetClient(fd)->GetFd(), " :No such channel\r\n"); tmp.erase(tmp.begin() + i--);}
		}
	return reason;
}

void	Server::KICK(std::string cmd, int fd)
{
	//ERR_BADCHANMASK (476) // if the channel mask is invalid
	std::vector<std::string> tmp;
	std::string reason ,user;
	reason = SplitCmdKick(cmd, tmp, user, fd);
	if (user.empty())
		{senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	for (size_t i = 0; i < tmp.size(); i++){ // search for the channel
		if (GetChannel(tmp[i])){// check if the channel exist
			Channel *ch = GetChannel(tmp[i]);
			if (!ch->get_client(fd) && !ch->get_admin(fd)) // check if the client is in the channel
				{senderror(442, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :You're not on that channel\r\n"); continue;}
			if(ch->get_admin(fd)){ // check if the client is admin
				if (ch->GetClientInChannel(user)){ // check if the client to kick is in the channel
					std::stringstream ss;
					ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName() << "@" << "localhost" << " KICK #" << tmp[i] << " " << user;
					if (!reason.empty())
						ss << " :" << reason << "\r\n";
					else ss << "\r\n";
					ch->sendTo_all(ss.str());
					std::cout << "		" << ss.str();
					if (ch->get_admin(ch->GetClientInChannel(user)->GetFd()))
						ch->remove_admin(ch->GetClientInChannel(user)->GetFd());
					else
						ch->remove_client(ch->GetClientInChannel(user)->GetFd());
					if (ch->GetClientsNumber() == 0)
						channels.erase(channels.begin() + i);
				}
				else // if the client to kick is not in the channel
					{senderror(441, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :They aren't on that channel\r\n"); continue;}
			}
			else // if the client is not admin
				{senderror(482, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :You're not channel operator\r\n"); continue;}
		}
		else // if the channel doesn't exist
			senderror(403, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :No such channel\r\n");
	}
}