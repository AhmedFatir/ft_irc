#include "../Server.hpp"

Client* Server::GetClientbynickname(std::string &nickname, Channel &channel)
{
	(void)channel;
	(void)nickname;
	return NULL;
	// for (size_t i = 0; i < channel.clients.size(); i++)
	// {
	// 	if (channel.clients[i].GetNickName() == nickname)
	// 		return &channel.clients[i];
	// }
}

void Server::Invite(std::string &cmd, int &fd)
{
	std::vector<std::string> scmd = split_cmd(cmd);
	if (!GetClient(fd) || GetClient(fd)->GetNickName().empty() || GetClient(fd)->GetUserName().empty()) //ERR_NOTREGISTERED (451) // if the client is not registered
		{senderror(451, "", fd, " :You have not registered\r\n"); return;}
	if(scmd.size() < 3)
		{senderror(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n"); return;}
	if(scmd[2][0] != '#' || !GetChannel(scmd[2]))
	    {senderror(403, "", fd, " :No such channel\r\n"); return;}
	if (!(GetChannel(scmd[2])->get_client(fd)) && !(GetChannel(scmd[2])->get_admin(fd)))
	    {senderror(442, scmd[2], fd, " :You're not on that channel\r\n"); return;}
	if (GetChannel(scmd[2])->GetClientInChannel(scmd[1]))
	    {senderror(443, GetClient(fd)->GetNickName(), scmd[2], fd, " :is already on channel\r\n"); return;}

	

}