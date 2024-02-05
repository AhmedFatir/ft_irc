#include "Bot.hpp"

// const char* api_key = "f1e01ffb8b2640a68bd145004240302";
const char* api_key = "LOzgqOjUO2qTsjgGPqDBB4LBDdIVGEzz";
const char* location = "London";

const char* host = "api.tomorrow.io";
const int port = 80;

void _sendMessage(std::string message, int fd)
{
    if(send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Send failed" << std::endl;
}

int main()
{
    int sockfd;
    struct sockaddr_in hints;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "socket failed\n" << std::endl;
        return 1;
    }
     struct hostent* server = gethostbyname(host);
    if (server == NULL) {
        perror("Error, no such host");
        close(sockfd);
        return 1;
    }
    memset(&hints, 0, sizeof(sockaddr_in));
    hints.sin_family = AF_INET;
    hints.sin_port = htons(port);
    // inet_pton(AF_INET, "127.0.0.1", &(hints.sin_addr));
    // if(connect(fd, (struct sockaddr*)&hints, sizeof(hints)) == -1) {
    //     std::cerr << "connect failed\n" << std::endl;
    //     return 1;
    // }
    memcpy((char*)&hints.sin_addr.s_addr, (char*)server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*)&hints, sizeof(hints)) < 0) {
        perror("Error connecting");
        close(sockfd);
        return 1;
    }

    // HTTP GET request
    std::string request = "GET /v1/current.json?key=" + std::string(api_key) + std::string(location) + "&aqi=no HTTP/1.1\r\n" + \
                          "Host: " + std::string(host) + "\r\n" + \
                          "Connection: close\r\n" + \
                          "\r\n";
    std::cout << request;

    if (send(sockfd, request.c_str(), request.size(), 0) < 0) {
        perror("Error writing to socket");
        close(sockfd);
        return 1;
    }

    // Receive and print the response
    char buffer[4096];
    ssize_t bytes_received;

    while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        std::cout << buffer;
    }

    if (bytes_received < 0) {
        perror("Error reading from socket");
    }


    // std::cout << "client connected: ... " << std::endl;
    // _sendMessage("PASS 123\r\n", fd);
    // _sendMessage("NICK bot\r\n", fd);
    // _sendMessage("USER bot 0 * bot\r\n", fd);

    // while(1);
}