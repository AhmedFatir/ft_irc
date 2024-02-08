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
	std::string created_at;
	bool topic_restriction;
	std::string topic_name;
	std::string password;
	std::string name;
	std::vector<Client> clients;
	std::vector<Client> admins;
	std::vector<std::pair<char, bool> > modes;
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

	bool Gettopic_restriction() const;
	int GetInvitOnly();
	int GetTopic();
	int GetKey();
	int GetLimit();
	//abdellah
	std::string clientChannel_list();
	void sendTo_all(std::string rpl1);
	void sendTo_all(std::string &rpl1, int fd);
	bool clientInChannel(std::string &nick);
	bool change_clientToAdmin(std::string& nick);
	bool change_adminToClient(std::string& nick);
	bool getModeAtindex(size_t index);
	void setModeAtindex(size_t index, bool mode);
	std::string getModes();
	void set_createiontime();
	std::string get_creationtime();
	void set_topicRestriction(bool value);
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
