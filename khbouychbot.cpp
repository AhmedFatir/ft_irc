#include "Server.hpp"

// Callback function to handle the response data
size_t Server::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output)
{
    size_t total_size = size * nmemb;
    output->append(static_cast<char *>(contents), total_size);
    return total_size;
}

void Server::khbouychbot(std::string cmd , int fd)
{

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
    std::string gusername;
    std::cout << "Enter the GitHub username: ";
    std::cin >> gusername;

    // Build the API URL with the user-input GitHub username
    std::string api_url = "https://api.github.com/users/" + gusername + "/repos";

    curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());

    // GitHub API requires a user-agent header
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "User-Agent: MyApplication/1.0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    res = curl_easy_perform(curl);

    // Cleanup headers
    curl_slist_free_all(headers);

    if (res != CURLE_OK)
    {
        std::cerr << "Error performing HTTP request: " << curl_easy_strerror(res) << std::endl;
    }
    else
    {
        // Parse and display the response (you might want to use a JSON library for proper parsing)
        std::cout << response_data << std::endl;
        char *rok = strtok((char *)response_data.c_str(), ",");
        while (rok != NULL)
        {
            std::cout << rok << std::endl;
            rok = strtok(NULL, ",");
        }
        // printdata(response_data);
    }

    curl_easy_cleanup(curl);
}
else
{
    std::cerr << "Error creating curl handle." << std::endl;
}
curl_global_cleanup();
}