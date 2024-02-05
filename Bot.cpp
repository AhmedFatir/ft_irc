#include "Bot.hpp"

const char* api_key = "LOzgqOjUO2qTsjgGPqDBB4LBDdIVGEzz";
const char* location = "London";

const char* host = "api.weatherapi.com";
// const int port = 80;

void _sendMessage(std::string message, int fd)
{
    if(send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Send failed" << std::endl;
}

std::string makeRequest()
{
    // std::stringstream ss;
    // ss.clear();

    // // ss << "GET /v1/current.json?key=" << std::string(API_KEY) << std::string(LOCATION) + "&aqi=no HTTP/1.1\r\n" << "Host: " + std::string(HOST) + "\r\n" << "Connection: close\r\n\r\n";
    // return ss.str();
    std::string request = "GET /v1/current.json?key=" + std::string(api_key) + std::string(location) + "&aqi=no HTTP/1.1\r\n"
                          "Host: " + std::string(host) + "\r\n"
                          "Connection: close\r\n"
                          "\r\n";
                        return request;
}

    // std::string request = "GET /v1/current.json?key=" + std::string(api_key) + std::string(location) + "&aqi=no HTTP/1.1\r\n"
    //                       "Host: " + std::string(host) + "\r\n"
    //                       "Connection: close\r\n"
    //                       "\r\n";

int main()
{
    // int ircsock;
    int apisock;
    struct hostent* apihost;
    // struct sockaddr_in ircHints;
    struct sockaddr_in apiHints;
    
    // ircsock = socket(AF_INET, SOCK_STREAM, 0);
    // if(ircsock == -1)
    // {
    //     std::cerr << "failed to create socket (ircsock)" << std::endl;
    //     return 1;
    // }

    apisock = socket(AF_INET, SOCK_STREAM, 0);
    if(apisock == -1)
    {
        std::cerr << "failed to create socket (apisock)" << std::endl;
        return 1;
    }
    // ircHints.sin_family = AF_INET;
    // ircHints.sin_port = htons(9009);
    // inet_pton(AF_INET, "127.0.0.1", &(ircHints.sin_addr));
    // if(connect(ircsock, (struct sockaddr*)&ircHints, sizeof(ircHints)) == -1) {
    //     std::cerr << "connect failed\n" << std::endl;
    //     return 1;
    // }
    // // connection to irc server
    // _sendMessage("PASS 123\r\n", ircsock);
    // _sendMessage("NICK bot\r\n", ircsock);
    // _sendMessage("USER bot 0 * bot\r\n", ircsock);

    // connection to the api socket
    apihost = gethostbyname("api.weatherapi.com");
    if(!apihost) {
        std::cerr << "gethostbyname failed\n" << std::endl;
        return 1;
    }
    apiHints.sin_family = AF_INET;
    apiHints.sin_port = htons(80);
    memcpy((char*)&apiHints.sin_addr.s_addr, apihost->h_addr, apihost->h_length);
    if(connect(apisock, (struct sockaddr*)&apiHints, sizeof(apiHints)) == -1) {
        std::cerr << "connect II failed\n" << std::endl;
        return 1;
    }
    std::string request = makeRequest();
    if(send(apisock,request.c_str(), request.size(), 0) == -1)
    {
        std::cerr << "send II failed\n" << std::endl;
        return 1;
    }
    std::cout << "request: " << request;


    char buffer[4096];
    ssize_t bytes_received;
    std::string rec;
    while ((bytes_received = recv(apisock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        rec += buffer;
    }

    std::cout << "recived: " << rec;



    // std::string resp;
    // ssize_t recivedBytes;
    // char buff[1024];
    // while( (recivedBytes = recv(ircsock, buff, sizeof(buff), 0)) > 0)
    // {
    //     buff[recivedBytes] = '\0';
    //     std::cout << "recived |" <<  buff;
        // resp = "PRIVMSG b hello wow wow wowow\r\n";
        // send(ircsock, resp.c_str(), resp.size(), 0);
        // resp.clear();
    // }
}