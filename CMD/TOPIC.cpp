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
	if (!GetClient(fd) || GetClient(fd)->GetNickName().empty() || GetClient(fd)->GetUserName().empty()) //ERR_NOTREGISTERED (451) // if the client is not registered
		{senderror(451, "", fd, " :You have not registered\r\n"); return;}
	std::vector<std::string> scmd = split_cmd(cmd);
	std::string nmch = scmd[1].substr(1);
	if(!GetChannel(nmch))
	    {senderror(403, scmd[1], fd, " :No such channel\r\n"); return;}
	if (!(GetChannel(nmch)->get_client(fd)) && !(GetChannel(nmch)->get_admin(fd)))
	    {senderror(442, scmd[1], fd, " :You're not on that channel\r\n"); return;}
	if (scmd.size() == 1)
		{senderror(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n"); return;}
	else if (scmd.size() == 2)
	{
		std::string respons = ":localhost 332 " + GetClient(fd)->GetUserName() + " " + scmd[1] + " :" + GetChannel(nmch)->GetTopicName() + "\r\n";
		GetChannel(nmch)->sendTo_all(nmch,fd);
		_sendResponse(respons, fd);
		std::string rep1 = ":localhost 333 " + GetClient(fd)->GetNickName() + " " + scmd[1] + " " + GetClient(fd)->GetNickName() + " " + tTopic() + "\r\n";
		GetChannel(nmch)->sendTo_all(nmch,fd);
		_sendResponse(rep1, fd);
	}

	else if (scmd.size() >= 3)
	{
		if (GetChannel(nmch)->Gettopic_restriction() && GetChannel(nmch)->get_client(fd))
		{
	    	senderror(442, nmch, fd, " :You're Not a channel operator\r\n");
			return;
		}
		else if (GetChannel(nmch)->Gettopic_restriction() && GetChannel(nmch)->get_admin(fd))
		{
			std::string restopic ;
			for (size_t i = 2; i < scmd.size(); i++)
				restopic += scmd[i] + " ";
			GetChannel(nmch)->SetTopicName(restopic);
			std::string respons= ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " :" + GetChannel(nmch)->GetTopicName() + "\r\n";
			GetChannel(nmch)->sendTo_all(nmch,fd);
			_sendResponse(respons, fd);
		}
		else
		{
			std::string restopic;
			for (size_t i = 2; i < scmd.size(); i++)
				restopic += scmd[i] + " ";
			GetChannel(nmch)->SetTopicName(restopic);
			std::string respons= ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " :" + GetChannel(nmch)->GetTopicName() + "\r\n";
			GetChannel(nmch)->sendTo_all(nmch,fd);
			_sendResponse(respons, fd);
		}
	}
}