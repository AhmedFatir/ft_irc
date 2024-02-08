#include "Server.hpp"

/* 
    PASS COMMAND
*/

void Server::client_authen(int fd, std::string& cmd)
{
	Client *cli = GetClient(fd);
	std::vector<std::string> splited_cmd = split_cmd(cmd);
	if(splited_cmd.size() < 2)
        _sendResponse(ERR_NOTENOUGHPARAM(std::string("nickname")), fd);
	else if(!cli->getRegistered())
	{
		std::string pass = splited_cmd[1];
		if(pass == password)
			cli->setRegistered(true);
		else
            _sendResponse(ERR_INCORPASS(std::string("nickname")), fd);
	}
	else
        _sendResponse(ERR_ALREADYREGISTERED(GetClient(fd)->GetNickName()), fd);
}


/* 
    NICK COMMAND
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

void Server::set_nickname(std::string& nickname, int fd)
{
	if(nickname == ":" || nickname.empty())
        _sendResponse(ERR_NONICKNAME(std::string("nickname")), fd);
	else if (nickNameInUse(nickname))
        _sendResponse(ERR_NICKINUSE(std::string("nickname")), fd);
	else if(!is_validNickname(nickname))
        _sendResponse(ERR_ERRONEUSNICK(std::string("nickname")), fd);
	else
	{
		Client *cli = GetClient(fd);
		if(cli && cli->GetNickName().empty() && cli->getRegistered())
		{
			std::string oldNick = cli->GetNickName();
			if(!oldNick.empty())
				_sendResponse(RPL_NICKCHANGE(oldNick,nickname), fd);
			cli->SetNickname(nickname);
		}
		else if (cli && !cli->getRegistered())
		{
			_sendResponse(ERR_NOTREGISTERED(nickname), fd);
			// RemoveFds(fd);
			// RemoveClient(fd);
			// close(fd);
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
		// close(fd);
		// RemoveFds(fd);
		// RemoveClient(fd);
	}
	else if (cli && !cli->GetUserName().empty())
		_sendResponse(ERR_ALREADYREGISTERED(cli->GetNickName()), fd);
	else
	{
		cli->SetUsername(splited_cmd[1]);
		_sendResponse(RPL_CONNECTED(cli->GetNickName()), fd);
	}
	std::cout << "ana heraaaaaaaaa" << std::endl;
}