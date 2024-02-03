#include "../Server.hpp"

void	SplitQuit(std::vector<std::string> &tmp, std::string cmd)
{
	std::istringstream stm(cmd);
	std::string str;
	while(stm >> cmd)
		tmp.push_back(cmd);
	tmp.erase(tmp.begin());
	for (size_t i = 0; i < tmp.size(); i++){//start from the first string and take the rest of the strings as the message
			for (size_t j = i; j < tmp.size(); j++)
				{str += " " + tmp[j];tmp.erase(tmp.begin() + j);j--;}
	}
	str.erase(str.begin());
	tmp.push_back(str);
	for (size_t i = 0; i < tmp.size(); i++)//erase the empty strings
		{if (tmp[i].empty())tmp.erase(tmp.begin() + i);}
}

void Server::QUIT(std::string cmd, int fd)
{
	if (!GetClient(fd) ||  GetClient(fd)->GetNickName().empty() || GetClient(fd)->GetUserName().empty()) //ERR_NOTREGISTERED (451) // if the client is not registered
		{senderror(451, "", fd, " :You have not registered\r\n"); return;}
	std::vector<std::string> tmp;
	SplitQuit(tmp, cmd);
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i].get_client(fd)){
			channels[i].remove_client(fd);
			if (channels[i].GetClientsNumber() == 0)
				channels.erase(channels.begin() + i);
			else{
				// :huhuh!~u@qk3i8byd6tfyg.irc QUIT :Quit: wow
				
				std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost QUIT " + ":Quit: " + tmp[0] + "\r\n";
				channels[i].sendTo_all(rpl);
				std::cout << "QUIT: " << GetClient(fd)->GetNickName() << " has left the channel #" << channels[i].GetName() << std::endl;
			}
		}
		else if (channels[i].get_admin(fd)){
			channels[i].remove_admin(fd);
			if (channels[i].GetClientsNumber() == 0)
				channels.erase(channels.begin() + i);
			else{
				std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost QUIT " + ":Quit: " + tmp[0] + "\r\n";
				channels[i].sendTo_all(rpl);
				std::cout << "QUIT: " << GetClient(fd)->GetNickName() << " has left the channel #" << channels[i].GetName() << std::endl;
			}
		}
	}
	std::cout << GetClient(fd)->GetNickName() << " has left the server " << ":Quit: " << tmp[0]  << std::endl;
	RemoveClient(fd);
	RemoveFds(fd);
	close(fd);
}