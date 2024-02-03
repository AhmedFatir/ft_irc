#include "Bot.hpp"

void _sendMessage(std::string message, int fd)
{
    if(send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Send failed" << std::endl;
}

// int main()
// {
//     int fd;
//     struct sockaddr_in hints;

//     if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//         std::cerr << "socket failed\n" << std::endl;
//         return 1;
//     }
//     memset(&hints, 0, sizeof(sockaddr_in));
//     hints.sin_family = AF_INET;
//     hints.sin_port = htons(9009);
//     inet_pton(AF_INET, "127.0.0.1", &(hints.sin_addr));
//     if(connect(fd, (struct sockaddr*)&hints, sizeof(hints)) == -1) {
//         std::cerr << "connect failed\n" << std::endl;
//         return 1;
//     }
//     std::cout << "client connected: ... " << std::endl;
//     _sendMessage("PASS 123\r\n", fd);
//     _sendMessage("NICK bot\r\n", fd);
//     _sendMessage("USER bot 0 * bot\r\n", fd);

//     while(1);
// }