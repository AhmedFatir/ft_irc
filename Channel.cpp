/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatir <afatir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 21:19:08 by afatir            #+#    #+#             */
/*   Updated: 2024/01/23 18:54:06 by afatir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(){}
Channel::~Channel(){}
Channel::Channel(Channel const &src){
	*this = src;
}
Channel &Channel::operator=(Channel const &src){
	if (this != &src){
		this->name = src.name;
		this->clients = src.clients;
		this->admins = src.admins;
	}
	return *this;
}

void Channel::SetName(std::string name){
	this->name = name;
}
std::string Channel::GetName(){
	return this->name;
}
void Channel::add_client(Client newClient){
	clients.push_back(newClient);
}
void Channel::add_admin(Client newClient){
	admins.push_back(newClient);
}
void Channel::remove_client(int fd){
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it){
		if (it->GetFd() == fd){
			clients.erase(it);
			break;
		}
	}
}
void Channel::remove_admin(int fd){
	for (std::vector<Client>::iterator it = admins.begin(); it != admins.end(); ++it){
		if (it->GetFd() == fd){
			admins.erase(it);
			break;
		}
	}
}