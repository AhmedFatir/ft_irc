#include "../Server.hpp"

void Server::Invite(std::string &cmd, int &fd)
{
	std::vector<std::string> scmd = split_cmd(cmd);
	if(scmd.size() < 3)
		{senderror(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n"); return;}
	std::cout << "|" << scmd[2]<< "|" << std::endl;
	std::string channelname = scmd[2].substr(1);
	if(scmd[2][0] != '#' || !GetChannel(channelname))
	    {senderror(403, channelname, fd, " :No such channel\r\n"); return;}
	if (!(GetChannel(channelname)->get_client(fd)) && !(GetChannel(channelname)->get_admin(fd)))
	    {senderror(442, channelname, fd, " :You're not on that channel\r\n"); return;}
	if (GetChannel(channelname)->GetClientInChannel(scmd[1]))
	    {senderror(443, GetClient(fd)->GetNickName(), channelname, fd, " :is already on channel\r\n"); return;}
	Client *clt = GetClientNick(scmd[1]);
	if (!clt)
	{
		senderror(401, scmd[1], fd, " :No such nick\r\n");
        return;
	}
	clt->AddChannelInvite(channelname);
	std::string rep1 = ": 341 "+ GetClient(fd)->GetNickName()+" "+ clt->GetNickName()+" "+scmd[2]+"\r\n";
	send(fd, rep1.c_str(), rep1.size(),0);
	std::string rep2 = ":"+ clt->getHostname() + " INVITE " + clt->GetNickName() + " " + scmd[2]+"\r\n";
	send(clt->GetFd(), rep1.c_str(), rep1.size(),0);
		// :testnet.ergo.chat 341 hehe heho #ccc
		// :hehe!~u@qk3i8byd6tfyg.irc INVITE heho #ccc
}