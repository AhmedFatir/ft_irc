#include <iostream>
#include <curl/curl.h>
#include <map>
#include <iostream>
#include <curl/curl.h>
#include <map>

// Callback function to handle the response data
size_t handelcallback(void* contents, size_t s, size_t n, std::string* o) {
    size_t t_size = s * n;
    o->append(static_cast<char*>(contents), t_size);
    return t_size;
}

void splitoutput(char *str, std::map<std::string, std::string> &m)
{
    std::string key;
    std::string value;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == ':')
        {
            key   = "";
            value = "";
            for (int j = 0; j < i; j++)
                if (str[j] != '"')
                    key += str[j];
            for (int j = i + 1; j < strlen(str); j++)
                if (str[j] != '"')
                    value += str[j];
            m[key] = value;
        }
    }
}

void printdata(std::map<std::string, std::string> &m)
{
    std::cout << "id         : " << m["id"] << std::endl;
    std::cout << "login      : " << m["{login"] << std::endl;
    std::cout << "name       : " << m["name"] << std::endl;
    std::cout << "bio        : " << m["bio"] << std::endl;
    std::cout << "location   : " << m["location"] << std::endl;
    std::cout << "avatar_url : " << m["avatar_url"] << std::endl;
}

int main() {
    CURL* curl;
    // Initialize libcurl
    CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK) {
        std::cerr << "Error initializing libcurl: " << curl_easy_strerror(res) << std::endl;
        return 1;
    }
    // Create a new curl handle
    curl = curl_easy_init();
    if (curl) {
        // Build the API URL with the user-input GitHub username
        std::string api_url = "https://api.github.com/users/khbouych";
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
        // GitHub API requires a user-agent header
        struct curl_slist* headers = NULL;
        // Set the user-agent header
        headers = curl_slist_append(headers, "User-Agent: MyApplication/1.0");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        std::string response_data;
        // Set the callback function to receive the response data
        //and pass the response_data object to the callback function as the fourth argument to the CURLOPT_WRITEDATA option
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handelcallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        // Perform the request
        res = curl_easy_perform(curl);
        // Cleanup headers
        curl_slist_free_all(headers);
        if (res != CURLE_OK) {
            std::cerr << "Error performing HTTP request: " << curl_easy_strerror(res) << std::endl;
        } else {
            char *rok = strtok((char *)response_data.c_str(), ",");
            std::map<std::string, std::string> m;
            while (rok != NULL)
            {
                splitoutput(rok, m);
                rok = strtok(NULL, ",");
            }
            printdata(m);
        }
    // Cleanup curl handle : free the resources used by the curl handle
    curl_easy_cleanup(curl);
    } else {
        std::cerr << "Error creating curl handle." << std::endl;
    }
    // Cleanup libcurl : free the resources used by libcurl globally
    curl_global_cleanup();
    return 0;
}


/*
    expected output:
    "id": 133979873
    "login": "khbouych"
    "name": "khalid bouychou"
    "avatar_url": "https://avatars.githubusercontent.com"
    "bio": "alx student"
    "location": "khouribga"
*/