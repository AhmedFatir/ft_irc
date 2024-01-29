/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khbouych <khbouych@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 21:14:29 by afatir            #+#    #+#             */
/*   Updated: 2024/01/28 14:31:24 by khbouych         ###   ########.fr       */
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
	int invit_only;
	int topic;
	int key;
	int limit;
	std::string topic_name;
	std::string password;
	std::string name;
	std::vector<Client> clients;
	std::vector<Client> admins;
public:
	Channel();
	~Channel();
	Channel(Channel const &src);
	Channel &operator=(Channel const &src);

	void SetInvitOnly(int invit_only);
	void SetTopic(int topic);
	void SetKey(int key);
	void SetLimit(int limit);
	void SetTopicName(std::string topic_name);
	void SetPassword(std::string password);
	void SetName(std::string name);

	int GetInvitOnly();
	int GetTopic();
	int GetKey();
	int GetLimit();
	//abdellah
	std::string clientChannel_list();
	void sendTo_all(std::string &rpl1, std::string &rpl2 , std::string &rpl3);
	void sendTo_all(std::string &rpl1);
	//
	std::string GetTopicName();
	std::string GetPassword();
	std::string GetName();

	void add_client(Client newClient);
	void add_admin(Client newClient);
	Client *get_client(int fd);
	Client *get_admin(int fd);
	void remove_client(int fd);
	void remove_admin(int fd);
	int GetClientsNumber();
	Client* GetClientInChannel(std::string name);
};

#endif
