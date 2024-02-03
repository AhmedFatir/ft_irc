/*
    aagouzou
*/
#include "Server.hpp"

//################################ MODE #####################################################


std::string Server::mode_toAppend(std::string chain, char opera, char mode)
{
	std::stringstream ss;

	ss.clear();
	char last = '\0';
	for(size_t i = 0; i < chain.size(); i++)
	{
		if(chain[i] == '+' || chain[i] == '-')
			last = chain[i];
	}
	if(last != opera)
		ss << opera << mode;
	else
		ss << mode;
	return ss.str();
}

void Server::mode_command(std::string& cmd, int fd)
{
	std::stringstream mode_chain;
	std::string arguments;
	char opera = '\0';
	
	arguments.clear();
	mode_chain.clear();
	std::vector<std::string> splited = split_cmd(cmd);
	Client *cli = GetClient(fd);
	if(!cli ||  cli->GetNickName().empty() || cli->GetUserName().empty()) // client not registered yet
	{
		std::string resp = ": 451 nickname :You have not registered\r\n";
		send(fd,resp.c_str(), resp.size(), 0);
		// close(fd);
		// RemoveFds(fd);
		// RemoveClient(fd);
		return ;
	}
	else if(splited.size() < 2) // Not enough parameters
	{
		_sendResponse(ERR_NOTENOUGHPARAM(cli->GetNickName()), fd); 
		return ;
	}	
		
	std::string channelName = splited[1].substr(1);
	Channel *channel = GetChannel(channelName);
	if(!channel) // No such channel
	{
		_sendResponse(ERR_CHANNELNOTFOUND(cli->GetUserName(),channel->GetName()), fd);
		return ;
	}
	else if (splited.size() == 2) // response with the channel modes (MODE #channel)
	{
		_sendResponse(RPL_CHANNELMODES(cli->GetNickName(), channel->GetName(), channel->getModes()) + \
		RPL_CREATIONTIME(cli->GetNickName(), channel->GetName(),channel->get_creationtime()),fd);
		return ;
	}
	else if (!channel->get_admin(fd)) // client is not channel operator
	{
		_sendResponse(ERR_NOTOPERATOR(channel->GetName()), fd);
		return ;
	}
	else if(channel)
	{
		size_t pos = 3;
		for(size_t i = 0; i < splited[2].size(); i++)
		{
			if(splited[2][i] == '+' || splited[2][i] == '-')
				opera = splited[2][i];
			else
			{
				if(splited[2][i] == 'i')//invite mode
					mode_chain << invite_only(channel , opera, mode_chain.str());
				else if (splited[2][i] == 't') //topic restriction mode
					mode_chain << topic_restriction(channel, opera, mode_chain.str());
				else if (splited[2][i] == 'k') //password set/remove
					mode_chain <<  password_mode(splited, channel, pos, opera, fd, mode_chain.str(), arguments);
				else if (splited[2][i] == 'o') //set/remove user operator privilege
						mode_chain << operator_privilege(splited, channel, pos, fd, opera, mode_chain.str(), arguments);
				else if (splited[2][i] == 'l') //set/remove channel limits
					mode_chain << channel_limit(splited, channel, pos, opera, fd, mode_chain.str(), arguments);
				else if (splited[2][i] == 's')
					mode_chain << mode_toAppend(mode_chain.str(),opera, 's');
				else if (splited[2][i] == 'n')
					mode_chain << mode_toAppend(mode_chain.str(),opera, 'n');
				else
					_sendResponse(ERR_UNKNOWNMODE(cli->GetNickName(), channel->GetName(),splited[2][i]),fd);
			}
		}
	}
	std::cout << "		mode: " << mode_chain.str() << std::endl;
	std::string chain = mode_chain.str();
	if(chain.empty())
		return ;
	channel->sendTo_all(RPL_CHANGEMODE(cli->getHostname(), channel->GetName(), mode_chain.str(), arguments));
}

std::string Server::invite_only(Channel *channel, char opera, std::string chain)
{
	std::string param;
	param.clear();
	if(opera == '+' && !channel->getModeAtindex(0))
	{
		channel->setModeAtindex(0, true);
		channel->SetInvitOnly(1);
		param =  mode_toAppend(chain, opera, 'i');
	}
	else if (opera == '-' && channel->getModeAtindex(0))
	{
		channel->setModeAtindex(0, false);
		channel->SetInvitOnly(0);
		param =  mode_toAppend(chain, opera, 'i');
	}
	return param;
}

std::string Server::topic_restriction(Channel *channel ,char opera, std::string chain)
{
	std::string param;
	param.clear();
	if(opera == '+' && !channel->getModeAtindex(1))
	{
		channel->setModeAtindex(1, true);
		channel->set_topicRestriction(true);
		param =  mode_toAppend(chain, opera, 't');
	}
	else if (opera == '-' && channel->getModeAtindex(1))
	{
		channel->setModeAtindex(1, false);
		channel->set_topicRestriction(false);
		param =  mode_toAppend(chain, opera, 't');
	}	
	return param;
}

std::string Server::password_mode(std::vector<std::string> splited, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string &arguments)
{
	std::string param;
	std::string pass;

	param.clear();
	pass.clear();
	if(splited.size() > pos)
		pass = splited[pos++];
	else
	{
		_sendResponse(ERR_NEEDMODEPARM(channel->GetName(),"(k)"),fd);
		return param;
	}
	if(opera == '+')
	{
		channel->setModeAtindex(2, true);
		channel->SetPassword(pass);
		if(!arguments.empty())
			arguments += " ";
		arguments += pass;
		param = mode_toAppend(chain,opera, 'k');
	}
	else if (opera == '-' && channel->getModeAtindex(2))
	{
		if(pass == channel->GetPassword())
		{		
			channel->setModeAtindex(2, false);
			channel->SetPassword("");
			param = mode_toAppend(chain,opera, 'k');
		}
		else
			_sendResponse(ERR_KEYSET(channel->GetName()),fd);
	}
	return param;
}

std::string Server::operator_privilege(std::vector<std::string> splited, Channel *channel, size_t& pos, int fd, char opera, std::string chain, std::string& arguments)
{
	std::string user;
	std::string param;

	param.clear();
	user.clear();
	if(splited.size() > pos)
		user = splited[pos++];
	else
	{
		_sendResponse(ERR_NEEDMODEPARM(channel->GetName(),"(o)"),fd);
		return param;
	}
	if(!channel->clientInChannel(user))
	{
		_sendResponse(ERR_NOSUCHNICK(channel->GetName(), user),fd);
		return param;
	}
	if(opera == '+')
	{
		channel->setModeAtindex(3,true);
		if(channel->change_clientToAdmin(user))
		{
			param = mode_toAppend(chain, opera, 'o');
			if(!arguments.empty())
				arguments += " ";
			arguments += user;
		}
	}
	else if (opera == '-')
	{
		channel->setModeAtindex(3,false);
		if(channel->change_adminToClient(user))
				param = mode_toAppend(chain, opera, 'o');
	}
	return param;
}

bool Server::isvalid_limit(std::string& limit)
{
	std::cout << std::atoi(limit.c_str()) << std::endl;
	return (!(limit.find_first_not_of("0123456789")!= std::string::npos) && std::atoi(limit.c_str()) > 0);
}

std::string Server::channel_limit(std::vector<std::string> splited,  Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string& arguments)
{
	std::string limit;
	std::string param;

	param.clear();
	limit.clear();
	if(opera == '+')
	{
		if(splited.size() > pos )
		{
			limit = splited[pos++];
			if(!isvalid_limit(limit))
			{
				_sendResponse(ERR_INVALIDMODEPARM(channel->GetName(),"(l)"), fd);
			}
			else
			{
				channel->setModeAtindex(4, true);
				channel->SetLimit(std::atoi(limit.c_str()));
				if(!arguments.empty())
					arguments += " ";
				arguments += limit;
				param =  mode_toAppend(chain, opera, 'l');
			}
		}
		else
			_sendResponse(ERR_NEEDMODEPARM(channel->GetName(),"(l)"),fd);
	}
	else if (opera == '-' && channel->getModeAtindex(4))
	{
		channel->setModeAtindex(4, false);
		channel->SetLimit(0);
		param  = mode_toAppend(chain, opera, 'l');
	}
	return param;
}