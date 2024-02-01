#include <vector>
#include <string>

// class Channel {
// public:
//     Channel(const std::string& name) : name("dddd") {}

//     std::string GetName() const {
//         return name;
//     }

//     // Add other channel properties/methods as needed

// private:
//     std::string name;
//     // Add other channel properties as needed
// };

// class Server {
// public:
//     Channel* GetChannel(std::string name);

//     // Add other server properties/methods as needed

// private:
//     std::vector<Channel> channels;
//     // Add other server properties as needed
// };

// Channel* Server::GetChannel(std::string name) {
//     for (size_t i = 0; i < this->channels.size(); i++) {
//         if (this->channels[i].GetName() == name)
//             return &this->channels[i];
//     }
//     return nullptr;
// }

#include <iostream>
#include <signal.h>
#include <unistd.h>

void SignalHandler(int signum)
{
	std::cout << std::endl << "Interrupt signal (" << signum << ") received." << std::endl;
	throw std::runtime_error("Proagram closed");
}

int main()
{
	try
	{    
		signal(SIGINT, SignalHandler);
		while (1)
		{
			std::cout << "Going to sleep...." << std::endl;
			sleep(1);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
