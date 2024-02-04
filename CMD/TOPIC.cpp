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
	if (!GetClient(fd) || GetClient(fd)->GetNickName().empty() || GetClient(fd)->GetUserName().empty()) //ERR_NOTREGISTERED (451) // if the client is not registered
		{senderror(451, "", fd, " :You have not registered\r\n"); return;}
	else if (scmd.size() < 2)
		return; //HANDL NOT ENOUGH PARAMTERS
	std::string nmch = getnamechannel(scmd[1]);
	Channel *ch = NULL;
	int chexist = 0;
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (nmch == channels[i].GetName())
		{
			ch = &channels[i];
			chexist = 1;
			break;
		}
		chexist = 0;
	}
	if (chexist)
	{
		Client *admin = ch->get_admin(fd);
		if (!admin)
		{
			std::string respons =  ":localhost 482 "+admin->GetNickName() + scmd[1] +" : You're Not a channel operator";
			send(fd, respons.c_str(), respons.size(),0);
		}
		Client *client = ch->get_client(fd);
		if (!client)
		{
			std::string respons =  ":localhost 482 "+admin->GetNickName() + scmd[1] +" : You are not in this channel";
			send(fd, respons.c_str(), respons.size(),0);
		}
		if (scmd.size() == 2)
		{
			if (admin)
			{
				std::string respons = ":localhost 332 " + admin->GetUserName() + " " + scmd[1] + " :" + ch->GetTopicName() + "\r\n";
				ch->sendTo_all(scmd[1],fd);
				send(fd, respons.c_str(), respons.size(),0);
				std::string rep1 = ":localhost 333 " + admin->GetNickName() + " " + scmd[1] + " " + admin->GetNickName() + " " + tTopic() + "\r\n";
				ch->sendTo_all(scmd[1],fd);
				send(fd, rep1.c_str(), rep1.size(),0);
			}
			else if (client)
			{
				std::string respons = ":localhost 332 " + client->GetUserName() + " " + scmd[1] + " :" + ch->GetTopicName() + "\r\n";
				ch->sendTo_all(scmd[1],fd);
				send(fd, respons.c_str(), respons.size(),0);
				std::string rep1 = ":localhost 333 " + client->GetNickName() + " " + scmd[1] + " " + client->GetNickName() + " " + tTopic() + "\r\n";
				ch->sendTo_all(scmd[1],fd);
				send(fd, rep1.c_str(), rep1.size(),0);
			}
		}

		if (scmd.size() == 3)
		{
			if (ch->Gettopic_restriction() && client)
			{
				std::string respons =  ":localhost 482 "+client->GetNickName() + scmd[1] +" : You're Not a channel operator";
				send(fd, respons.c_str(), respons.size(),0);
			}
			else if (ch->Gettopic_restriction() && admin)
			{
				std::string restopic ;
				for (size_t i = 2; i < scmd.size(); i++)
					restopic += scmd[i] + " ";
				ch->SetTopicName(restopic);
				std::string respons= ":" + admin->GetNickName() + "!" + admin->GetUserName() + "@localhost TOPIC #" + nmch + " :" + ch->GetTopicName() + "\r\n";
				ch->sendTo_all(scmd[1],fd);
				send(fd, respons.c_str(), respons.size(),0);
			}
			else
			{
				if (admin)
				{
					std::string restopic;
					for (size_t i = 2; i < scmd.size(); i++)
						restopic += scmd[i] + " ";
					ch->SetTopicName(restopic);
					std::string respons= ":" + admin->GetNickName() + "!" + admin->GetUserName() + "@localhost TOPIC #" + nmch + " :" + ch->GetTopicName() + "\r\n";
					ch->sendTo_all(scmd[1],fd);
					send(fd, respons.c_str(), respons.size(),0);
				}
				else if (client)
				{
					std::string restopic ;
					for (size_t i = 2; i < scmd.size(); i++)
						restopic += scmd[i] + " ";
					ch->SetTopicName(restopic);
					std::string respons= ":" + client->GetNickName() + "!" + client->GetUserName() + "@localhost TOPIC #" + nmch + " :" + ch->GetTopicName() + "\r\n";
					ch->sendTo_all(scmd[1],fd);
					send(fd, respons.c_str(), respons.size(),0);
				}
			}
		}
	}
	else
	{
		std::string respons = ":localhost 403 " + GetClient(fd)->GetNickName() + " " + nmch + " :No such channel\r\n";
		send(fd, respons.c_str(), respons.size(),0);
	}
}