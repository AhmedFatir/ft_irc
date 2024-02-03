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
	(void)fd;
	std::vector<std::string> scmd = split_cmd(cmd);
	Channel *ch = GetChannel(scmd[2]);

	Client *clt = GetClientbynickname(scmd[1], *ch);

	if (clt)
	{
		std::cout << "--> nickname : " + clt->GetNickName() << std::endl;
	}
	else
	{
		puts("---> nop");
	}

}