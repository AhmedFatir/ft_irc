#include "../Server.hpp"

void Server::Invite(std::string &cmd, int &fd)
{
	std::vector<std::string> scmd = split_cmd(cmd);
	if (!GetClient(fd) || GetClient(fd)->GetNickName().empty() || GetClient(fd)->GetUserName().empty()) //ERR_NOTREGISTERED (451) // if the client is not registered
		{senderror(451, "", fd, " :You have not registered\r\n"); return;}
	if(scmd.size() < 3)// ERR_NEEDMOREPARAMS (461) if there are not enough parameters
		{senderror(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n"); return;}
	std::string channelname = scmd[2].substr(1);
	if(scmd[2][0] != '#' || !GetChannel(channelname))// ERR_NOSUCHCHANNEL (403) if the given channel does not exist
	    {senderror(403, channelname, fd, " :No such channel\r\n"); return;}
	if (!(GetChannel(channelname)->get_client(fd)) && !(GetChannel(channelname)->get_admin(fd)))// ERR_NOTONCHANNEL (442) if the client is not on the channel
	    {senderror(442, channelname, fd, " :You're not on that channel\r\n"); return;}
	if (GetChannel(channelname)->GetClientInChannel(scmd[1]))// ERR_USERONCHANNEL (443) if the given nickname is already on the channel
	    {senderror(443, GetClient(fd)->GetNickName(), channelname, fd, " :is already on channel\r\n"); return;}
	Client *clt = GetClientNick(scmd[1]);
	if (!clt)// ERR_NOSUCHNICK (401) if the given nickname is not found
		{senderror(401, scmd[1], fd, " :No such nick\r\n");return;}
	if (GetChannel(channelname)->GetInvitOnly() && !GetChannel(channelname)->get_admin(fd))// ERR_INVITEONLYCHAN (473) if the channel is invite-only
		{senderror(482,GetChannel(channelname)->get_client(fd)->GetNickName(),scmd[1],fd," :You're not channel operator\r\n"); return;}
	if (GetChannel(channelname)->GetLimit() && GetChannel(channelname)->GetClientsNumber() >= GetChannel(channelname)->GetLimit()) // ERR_CHANNELISFULL (471) if the channel is full
		{senderror(473,GetChannel(channelname)->get_client(fd)->GetNickName(),channelname,fd," :Cannot join channel (+l)\r\n"); return;}
	// RPL_INVITING (341) if the invite was successfully sent
	clt->AddChannelInvite(channelname);
	std::string rep1 = ": 341 "+ GetClient(fd)->GetNickName()+" "+ clt->GetNickName()+" "+scmd[2]+"\r\n";
	send(fd, rep1.c_str(), rep1.size(),0);
	std::string rep2 = ":"+ clt->getHostname() + " INVITE " + clt->GetNickName() + " " + scmd[2]+"\r\n";
	send(clt->GetFd(), rep1.c_str(), rep1.size(),0);
}