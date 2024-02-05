#include "../Server.hpp"

std::string Server::tTopic()
{
	std::time_t current = std::time(NULL);
	std::stringstream res;

	res << current;
	return res.str();
}
void Server::Topic(std::string &cmd, int &fd)
{
	std::vector<std::string> scmd = split_cmd(cmd);
	std::string nmch = scmd[1].substr(1);
	if (!GetClient(fd) || GetClient(fd)->GetNickName().empty() || GetClient(fd)->GetUserName().empty()) //ERR_NOTREGISTERED (451) // if the client is not registered
		{senderror(451, "", fd, " :You have not registered\r\n"); return;}
	if((scmd.size() == 2 && nmch == ":")|| !GetChannel(nmch))
	    {senderror(403, "#"+nmch, fd, " :No such channel\r\n"); return;}
	if (!(GetChannel(nmch)->get_client(fd)) && !(GetChannel(nmch)->get_admin(fd)))
	    {senderror(442, "#"+nmch, fd, " :You're not on that channel\r\n"); return;}
	if (scmd.size() == 2)
	{_sendResponse(": 332 " + GetClient(fd)->GetUserName() + " " + "#"+nmch + " :" + GetChannel(nmch)->GetTopicName() + "\r\n", fd);_sendResponse(": 333 " + GetClient(fd)->GetNickName() + " " + "#"+nmch + " " + GetClient(fd)->GetNickName() + " " + tTopic() + "\r\n", fd);return;}
	if (scmd.size() >= 3)
	{
		std::string restopic ;
		for (size_t i = 2; i < scmd.size(); i++)
			restopic += scmd[i] + " ";
		if (GetChannel(nmch)->Gettopic_restriction() && GetChannel(nmch)->get_client(fd))
		{senderror(442, nmch, fd, " :You're Not a channel operator\r\n");return;}
		else if (GetChannel(nmch)->Gettopic_restriction() && GetChannel(nmch)->get_admin(fd))
		{GetChannel(nmch)->SetTopicName(restopic);GetChannel(nmch)->sendTo_all(nmch,fd);_sendResponse(":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " :" + GetChannel(nmch)->GetTopicName() + "\r\n", fd);}
		else
			{GetChannel(nmch)->SetTopicName(restopic);GetChannel(nmch)->sendTo_all(nmch,fd);_sendResponse(":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " :" + GetChannel(nmch)->GetTopicName() + "\r\n", fd);}
	}
}