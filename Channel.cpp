/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatir <afatir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 21:19:08 by afatir            #+#    #+#             */
/*   Updated: 2024/01/22 21:22:09 by afatir           ###   ########.fr       */
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