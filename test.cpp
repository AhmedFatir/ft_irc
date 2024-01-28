#include <iostream>
#include <vector>
#include <string>

class Channel {
public:
    Channel(const std::string& name) : name("dddd") {}

    std::string GetName() const {
        return name;
    }

    // Add other channel properties/methods as needed

private:
    std::string name;
    // Add other channel properties as needed
};

class Server {
public:
    Channel* GetChannel(std::string name);

    // Add other server properties/methods as needed

private:
    std::vector<Channel> channels;
    // Add other server properties as needed
};

Channel* Server::GetChannel(std::string name) {
    for (size_t i = 0; i < this->channels.size(); i++) {
        if (this->channels[i].GetName() == name)
            return &this->channels[i];
    }
    return nullptr;
}

int main() {
    // Example usage
    Server server;
    server.GetChannel("dddd")->GetName();
    // Assume channels have been added to the server

    return 0;
}
