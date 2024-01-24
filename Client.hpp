/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatir <afatir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 21:13:42 by afatir            #+#    #+#             */
/*   Updated: 2024/01/23 18:55:35 by afatir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include "Channel.hpp"


class Client
{
private:
	std::string nickname;
	std::string username;
	bool isOperator;
	int fd;
public:
	Client();
	Client(std::string nickname, std::string username, int fd);
	~Client();
	Client(Client const &src);
	Client &operator=(Client const &src);
	
	std::string GetNickName();
	std::string GetUserName();
	void SetFd(int fd);
	int GetFd();

	//###############
	void SetNickname(std::string& nickName);
};

#endif