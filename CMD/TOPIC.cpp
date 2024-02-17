#include "../INC/Server.hpp"

std::string Server::tTopic()
{
	std::time_t current = std::time(NULL);
	std::stringstream res;

	res << current;
	return res.str();
}
std::string Server::gettopic(std::string& input)
{
    size_t pos = input.find(":");
    if (pos == std::string::npos) {
        return "";
    }
    return input.substr(pos);
}

void Server::Topic(std::string &cmd, int &fd)
{
	std::vector<std::string> scmd = split_cmd(cmd);
	std::vector<std::string> tmp;
	std::string topic = gettopic(cmd);
	std::cout << "TOPIC" << topic <<std::endl;
	// if (topic != "")
	// {
	// 	tmp.push_back(scmd[0]);
	// 	tmp.push_back(scmd[1]);
	// 	tmp.push_back(topic);
	// }
	// else
	// {
	// 	tmp.push_back(scmd[0]);
	// 	tmp.push_back(scmd[1]);
	// 	tmp.push_back(scmd[2]);
	// }
	// if (scmd.size() == 1)
	// 	{senderror(403, "", fd, " :No such channel\r\n"); return;}
	// std::string nmch = scmd[1].substr(1);
	// if(!GetChannel(nmch))
	//     {senderror(403, "#"+nmch, fd, " :No such channel\r\n"); return;}
	// // if((scmd.size() == 2 && nmch == ":")|| !GetChannel(nmch))
	// //     {senderror(403, "#"+nmch, fd, " :No such channel\r\n"); return;}
	// if (!(GetChannel(nmch)->get_client(fd)) && !(GetChannel(nmch)->get_admin(fd)))
	//     {senderror(442, "#"+nmch, fd, " :You're not on that channel\r\n"); return;}
	// if (topic == ":" || (scmd.size() == 2 && nmch == ":"))
	// 	{senderror(331, "#"+nmch, fd, " :No topic is set\r\n"); return;}
	// if (scmd.size() == 2)
	// {
	// 	if (GetChannel(nmch)->GetTopicName() == "")
	// 		{_sendResponse(": 331 " + GetClient(fd)->GetNickName() + " " + "#"+nmch + " :No topic is set\r\n", fd);return;}
	// 	size_t pos = GetChannel(nmch)->GetTopicName().find(":");
	// 	if (GetChannel(nmch)->GetTopicName() != "" && pos == std::string::npos)
	// 	{
	// 		_sendResponse(": 332 " + GetClient(fd)->GetNickName() + " " + "#"+nmch  +" :"+ GetChannel(nmch)->GetTopicName() + "\r\n", fd);
	// 		_sendResponse(": 333 " + GetClient(fd)->GetNickName() + " " + "#"+nmch + " " + GetClient(fd)->GetNickName() + " " + tTopic() + "\r\n", fd);
	// 		return;
	// 	}
	// 	else
	// 		{
	// 			_sendResponse(": 332 " + GetClient(fd)->GetNickName() + " " + "#"+nmch + " " + GetChannel(nmch)->GetTopicName() + "\r\n", fd);
	// 			_sendResponse(": 333 " + GetClient(fd)->GetNickName() + " " + "#"+nmch + " " + GetClient(fd)->GetNickName() + " " + tTopic() + "\r\n", fd);
	// 			return;
	// 		}
	// }
	// if (scmd.size() >= 3)
	// {
	// 	if (GetChannel(nmch)->Gettopic_restriction() && GetChannel(nmch)->get_client(fd))
	// 		{senderror(442, nmch, fd, " :You're Not a channel operator\r\n");return;}
	// 	else if (GetChannel(nmch)->Gettopic_restriction() && GetChannel(nmch)->get_admin(fd))
	// 	{
	// 		GetChannel(nmch)->SetTopicName(tmp[2]);
	// 		std::string rpl;
	// 		size_t pos = cmd.find(":");
	// 		if (pos == std::string::npos)
	// 			rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " :" + GetChannel(nmch)->GetTopicName() + "\r\n";
	// 		else
	// 			rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch +" "+ GetChannel(nmch)->GetTopicName() + "\r\n";
	// 		GetChannel(nmch)->sendTo_all(rpl);
	// 	}
	// 	else
	// 	{
	// 		GetChannel(nmch)->SetTopicName(tmp[2]);
	// 		std::string rpl;
	// 		if (topic == "")
	// 			rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " :" + GetChannel(nmch)->GetTopicName() + "\r\n";
	// 		else
	// 			rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch +" "+ GetChannel(nmch)->GetTopicName() + "\r\n";
	// 		GetChannel(nmch)->sendTo_all(rpl);
	// 	}
	// }
}