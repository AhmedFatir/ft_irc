#include "../INC/Server.hpp"

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

void Server::getCmdArgs(std::string cmd,std::string& name, std::string& modeset ,std::string &params)
{
	std::istringstream stm(cmd);
	stm >> name;
	stm >> modeset;
	size_t found = cmd.find_first_not_of(name + modeset + " \t\v");
	if(found != std::string::npos)
		params = cmd.substr(found);
}


std::vector<std::string> Server::splitParams(std::string params)
{
	if(!params.empty() && params[0] == ':')
		params.erase(params.begin());
	std::vector<std::string> tokens;
	std::string param;
	std::istringstream stm(params);
	while (std::getline(stm, param, ','))
	{
		tokens.push_back(param);
		param.clear();
	}
	return tokens;
}

void Server::mode_command(std::string& cmd, int fd)
{
	std::string channelName;
	std::string params;
	std::string modeset;
	std::stringstream mode_chain;
	std::string arguments = ":";
	Channel *channel;
	char opera = '\0';

	arguments.clear();
	mode_chain.clear();
	Client *cli = GetClient(fd);
	size_t found = cmd.find_first_not_of("MODEmode \t\v");
	if(found != std::string::npos)
		cmd = cmd.substr(found);
	else
	{
		_sendResponse(ERR_NOTENOUGHPARAM(cli->GetNickName()), fd); 
		return ;
	}
	getCmdArgs(cmd ,channelName, modeset ,params);
	std::vector<std::string> tokens = splitParams(params);
	if(channelName[0] != '#' || !(channel = GetChannel(channelName.substr(1))))
	{
		_sendResponse(ERR_CHANNELNOTFOUND(cli->GetUserName(),channelName), fd);
		return ;
	}
	else if (!channel->get_client(fd) && !channel->get_admin(fd))
	{
		senderror(442, GetClient(fd)->GetNickName(), channelName, GetClient(fd)->GetFd(), " :You're not on that channel\r\n"); return;
	}
	else if (modeset.empty()) // response with the channel modes (MODE #channel)
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
		size_t pos = 0;
		for(size_t i = 0; i < modeset.size(); i++)
		{
			if(modeset[i] == '+' || modeset[i] == '-')
				opera = modeset[i];
			else
			{
				if(modeset[i] == 'i')//invite mode
					mode_chain << invite_only(channel , opera, mode_chain.str());
				else if (modeset[i] == 't') //topic restriction mode
					mode_chain << topic_restriction(channel, opera, mode_chain.str());
				else if (modeset[i] == 'k') //password set/remove
					mode_chain <<  password_mode(tokens, channel, pos, opera, fd, mode_chain.str(), arguments);
				else if (modeset[i] == 'o') //set/remove user operator privilege
						mode_chain << operator_privilege(tokens, channel, pos, fd, opera, mode_chain.str(), arguments);
				else if (modeset[i] == 'l') //set/remove channel limits
					mode_chain << channel_limit(tokens, channel, pos, opera, fd, mode_chain.str(), arguments);
				else
					_sendResponse(ERR_UNKNOWNMODE(cli->GetNickName(), channel->GetName(),modeset[i]),fd);
			}
		}
	}
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

bool validPassword(std::string password)
{
	if(password.empty())
		return false;
	for(size_t i = 0; i < password.size(); i++)
	{
		if(!std::isalnum(password[i]) && password[i] != '_')
			return false;
	}
	return true;
}
std::string Server::password_mode(std::vector<std::string> tokens, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string &arguments)
{
	std::string param;
	std::string pass;

	param.clear();
	pass.clear();
	if(tokens.size() > pos)
		pass = tokens[pos++];
	else
	{
		_sendResponse(ERR_NEEDMODEPARM(channel->GetName(),std::string("(k)")),fd);
		return param;
	}
	if(!validPassword(pass))
	{
		_sendResponse(ERR_INVALIDMODEPARM(channel->GetName(),std::string("(k)")),fd);
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

std::string Server::operator_privilege(std::vector<std::string> tokens, Channel *channel, size_t& pos, int fd, char opera, std::string chain, std::string& arguments)
{
	std::string user;
	std::string param;

	param.clear();
	user.clear();
	if(tokens.size() > pos)
		user = tokens[pos++];
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
		{
			param = mode_toAppend(chain, opera, 'o');
				if(!arguments.empty())
					arguments += " ";
			arguments += user;

		}
	}
	return param;
}

bool Server::isvalid_limit(std::string& limit)
{
	return (!(limit.find_first_not_of("0123456789")!= std::string::npos) && std::atoi(limit.c_str()) > 0);
}

std::string Server::channel_limit(std::vector<std::string> tokens,  Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string& arguments)
{
	std::string limit;
	std::string param;

	param.clear();
	limit.clear();
	if(opera == '+')
	{
		if(tokens.size() > pos )
		{
			limit = tokens[pos++];
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