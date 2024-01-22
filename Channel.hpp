/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatir <afatir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 21:14:29 by afatir            #+#    #+#             */
/*   Updated: 2024/01/22 21:42:01 by afatir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include "Server.hpp"

class Client;

class Channel
{
private:
	std::string name;
	std::vector<Client> clients;
public:
	Channel();
	~Channel();
	Channel(Channel const &src);
	Channel &operator=(Channel const &src);

	void SetName(std::string name);
	std::string GetName();
	void add_client(Client newClient);
};

#endif
