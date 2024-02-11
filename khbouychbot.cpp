#include "Server.hpp"
static size_t handelcallback(void *contents, size_t s, size_t n, std::string *o)
{
    size_t t_size = s * n;
    o->append(static_cast<char *>(contents), t_size);
    return t_size;
}

void Server::splitoutput(char *str, std::map<std::string, std::string> &m)
{
    std::string key;
    std::string value;
    for (int i = 0; i < (int)strlen(str); i++)
    {
        if (str[i] == ':')
        {
            key = "";
            value = "";
            for (int j = 0; j < i; j++)
                if (str[j] != '"')
                    key += str[j];
            for (int j = i + 1; j < (int)strlen(str); j++)
                if (str[j] != '"')
                    value += str[j];
            m[key] = value;
        }
    }
}

void Server::printdata(std::map<std::string, std::string> &m, int &fd)
{
    std::string id = "id"+ std::string(" : ") +m["id"];
    std::cout << "id         : " << m["id"] << std::endl;
    std::string login = "login"+ std::string(" : ") +m["login"];
    std::cout << "login      : " << m["login"] << std::endl;
    std::string name = "name"+ std::string(" : ") +m["name"];
    std::cout << "name       : " << m["name"] << std::endl;
    std::string bio = "bio"+ std::string(" : ") +m["bio"];
    std::cout << "bio        : " << m["bio"] << std::endl;
    std::string location = "location"+ std::string(" : ") +m["location"];
    std::cout << "location   : " << m["location"] << std::endl;
    std::string avatar_url = "avatar_url"+ std::string(" : ") +m["avatar_url"];
    std::cout << "avatar_url : " << m["avatar_url"] << std::endl;
    std::string rep = id + "\n" + login + "\n" + name + "\n" + bio + "\n" + location + "\n" + avatar_url + "\n";
    send(fd, rep.c_str(), rep.size(), 0);
}

void Server::botgithub(std::string cmd, int fd)
{
    std::vector<std::string> scmd = split_cmd(cmd);
    if (scmd.size() < 2 || scmd.size() > 2)
        senderror(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n");return;
    std::cout << "scmd.size() = " << scmd.size() << std::endl;
    std::map<std::string, std::string> m;
    std::string username = scmd[1];
    std::string api_url = "https://api.github.com/users/" + username;
    CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK)
    {
        std::string error = "Error initializing libcurl: " + std::string(curl_easy_strerror(res)) + "\r\n";
        senderror(461, GetClient(fd)->GetNickName(), fd, error);
        // std::cerr << "Error initializing libcurl: " << curl_easy_strerror(res) << std::endl;return;
    }
    CURL *curl;
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "User-Agent: MyApplication/1.0");
        if (headers == NULL)
        {
            std::string error = "Error appending headers.\r\n";
            senderror(461, GetClient(fd)->GetNickName(), fd, error);
            // std::cerr << "Error appending headers." << std::endl;return;
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        std::string response_data;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handelcallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        if (response_data.empty() == false)
        {
            std::string error = "Error response_data is not empty.\r\n";
            senderror(461, GetClient(fd)->GetNickName(), fd, error);
            response_data.clear();
        }
        curl_slist_free_all(headers);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::string error = "Error performing HTTP request: " + std::string(curl_easy_strerror(res)) + "\r\n";
            senderror(461, GetClient(fd)->GetNickName(), fd, error);
            // std::cerr << "Error performing HTTP request: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            char *rok = strtok((char *)response_data.c_str(), ",");
            if (rok == NULL)
            {
                std::string error = "Error strtok response_data.\r\n";
                senderror(461, GetClient(fd)->GetNickName(), fd, error);
                // std::cerr << "Error strtok response_data." << std::endl;
            }
            while (rok != NULL)
                splitoutput(rok, m);rok = strtok(NULL, ",");
            printdata(m, fd);
        }
        curl_easy_cleanup(curl);
    }
    else
    {
        std::string error = "Error creating curl handle.\r\n";
        senderror(461, GetClient(fd)->GetNickName(), fd, error);
        // std::cerr << "Error creating curl handle." << std::endl;
    }
    curl_global_cleanup();
}

/*
CURL *curl;
    CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK)
    {
        std::cerr << "Error initializing libcurl: " << curl_easy_strerror(res) << std::endl;
        return;
    }
    curl = curl_easy_init();
    if (curl)
    {
        std::string api_url = "https://api.github.com/users/khbouych";
        curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "User-Agent: MyApplication/1.0");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        std::string response_data;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handelcallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        if (res != CURLE_OK)
            std::cerr << "Error performing HTTP request: " << curl_easy_strerror(res) << std::endl;
        else
        {
            char *rok = strtok((char *)response_data.c_str(), ",");
            std::map<std::string, std::string> m;
            while (rok != NULL)
            {
                splitoutput(rok, m);
                rok = strtok(NULL, ",");
            }
            printdata(m);
        }
        curl_easy_cleanup(curl);
    }
    else
        std::cerr << "Error creating curl handle." << std::endl;
    curl_global_cleanup();
*/