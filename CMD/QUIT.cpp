#include "../Server.hpp"

void	SplitQuit(std::vector<std::string> &tmp, std::string cmd)
{
	std::istringstream stm(cmd);
	std::string str;
	while(stm >> cmd)
		tmp.push_back(cmd);
	tmp.erase(tmp.begin());
	if (tmp.empty())
		{tmp.push_back("Quit"); return;}
	for (size_t i = 0; i < tmp.size(); i++){//start from the first string and take the rest of the strings as the message
			for (size_t j = i; j < tmp.size(); j++)
				{str += " " + tmp[j];tmp.erase(tmp.begin() + j);j--;}
	}
	str.erase(str.begin());
	if (str[0] == ':') //if the message starts with ':'
		{str.insert(str.begin()+1 , ' ');}
	else if (str[0] != ':'){ //if the message does not start with ':'
		for (size_t i = 0; i < str.size(); i++){
			if (str[i] == ' ')
				{str.erase(str.begin() + i, str.end());break;}
		}
		str.insert(str.begin(), ' ');
		str.insert(str.begin(), ':');
	}
	str = ":Quit" + str;
	tmp.push_back(str);
	for (size_t i = 0; i < tmp.size(); i++)//erase the empty strings
		{if (tmp[i].empty())tmp.erase(tmp.begin() + i--);}
}

void Server::QUIT(std::string cmd, int fd)
{
	std::vector<std::string> tmp;
	SplitQuit(tmp, cmd);
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i].get_client(fd)){
			channels[i].remove_client(fd);
			if (channels[i].GetClientsNumber() == 0)
				channels.erase(channels.begin() + i);
			else{
				std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost QUIT " + tmp[0] + "\r\n";
				channels[i].sendTo_all(rpl);
				std::cout << "QUIT: " << GetClient(fd)->GetNickName() << " has left the channel #" << channels[i].GetName() << std::endl;
			}
		}
		else if (channels[i].get_admin(fd)){
			channels[i].remove_admin(fd);
			if (channels[i].GetClientsNumber() == 0)
				channels.erase(channels.begin() + i);
			else{
				std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost QUIT " + tmp[0] + "\r\n";
				channels[i].sendTo_all(rpl);
				std::cout << "QUIT: " << GetClient(fd)->GetNickName() << " has left the channel #" << channels[i].GetName() << std::endl;
			}
		}
	}
	std::cout << GetClient(fd)->GetNickName() << " has left the server " << tmp[0]  << std::endl;
	RemoveClient(fd);
	RemoveFds(fd);
	close(fd);
}