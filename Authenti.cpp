#include "Server.hpp"

/* 
*   PASS COMMAND
*/

void Server::client_authen(int fd, std::string cmd)
{
	Client *cli = GetClient(fd);
	cmd = cmd.substr(4);
	cmd.erase(cmd.begin());
	if(cmd.empty())
        _sendResponse(ERR_NOTENOUGHPARAM(std::string("nickname")), fd);
	else if(!cli->getRegistered())
	{
		std::string pass = cmd;
		if(pass == password)
			cli->setRegistered(true);
		else
            _sendResponse(ERR_INCORPASS(std::string("nickname")), fd);
	}
	else
        _sendResponse(ERR_ALREADYREGISTERED(GetClient(fd)->GetNickName()), fd);
}


/* 
*    NICK COMMAND
*/

bool Server::is_validNickname(std::string& nickname)
{
	if(!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':' || std::isdigit(nickname[0])))
		return false;
	for(size_t i = 1; i < nickname.size(); i++)
	{
		if(!std::isalnum(nickname[i]) && nickname[i] != '{' && nickname[i] != '}' && \
			nickname[i] != '[' && nickname[i] != ']' && nickname[i] != '|' && nickname[i] != '\\')
			return false;
	}
	return true;
}


bool Server::nickNameInUse(std::string& nickname)
{
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (this->clients[i].GetNickName() == nickname)
			return true;
	}
	return false;
}

void Server::set_nickname(std::string cmd, int fd)
{
	cmd = cmd.substr(4);
	cmd.erase(cmd.begin());
	if(cmd.empty()|| cmd == ":")
        _sendResponse(ERR_NONICKNAME(std::string("nickname")), fd);
	else if (nickNameInUse(cmd))
        _sendResponse(ERR_NICKINUSE(std::string("nickname")), fd);
	else if(!is_validNickname(cmd))
        _sendResponse(ERR_ERRONEUSNICK(std::string("nickname")), fd);
	else
	{
		Client *cli = GetClient(fd);
		if(cli && cli->getRegistered())
		{
			std::string oldNick = cli->GetNickName();
			if(!oldNick.empty())
				_sendResponse(RPL_NICKCHANGE(oldNick,cmd), fd);
			cli->SetNickname(cmd);
		}
		else if (cli && !cli->getRegistered())
		{
			_sendResponse(ERR_NOTREGISTERED(cmd), fd);
		}
	}
}

/* 
    USER COMMAND
*/

void	Server::set_username(std::string& cmd, int fd)
{
	std::vector<std::string> splited_cmd = split_cmd(cmd);

	Client *cli = GetClient(fd); 
	if((cli && splited_cmd.size() < 5))
		_sendResponse(ERR_NOTENOUGHPARAM(cli->GetNickName()), fd);
	else if(!cli || (cli && cli->GetNickName().empty()) || !cli->getRegistered())
	{
		_sendResponse(ERR_NOTREGISTERED(std::string("nickname")), fd);
	}
	else if (cli && !cli->GetUserName().empty())
		_sendResponse(ERR_ALREADYREGISTERED(cli->GetNickName()), fd);
	else
	{
		cli->SetUsername(splited_cmd[1]);
		_sendResponse(RPL_CONNECTED(cli->GetNickName()), fd);
	}
}