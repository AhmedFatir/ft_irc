#include <iostream>
#include <curl/curl.h>
#include <map>
#include <iostream>
#include <curl/curl.h>
#include <map>

// Callback function to handle the response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

void splitoutput(char *str, std::map<std::string, std::string> &m)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == ':')
        {
            std::string key = "";
            std::string value = "";
            for (int j = 0; j < i; j++)
            {
                if (str[j] != '"')
                {
                    key += str[j];
                }
            }
            for (int j = i + 1; j < strlen(str); j++)
            {
                if (str[j] != '"')
                {
                    value += str[j];
                }
            }
            m[key] = value;
        }
    }
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
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        // Perform the request
        res = curl_easy_perform(curl);
        // Cleanup headers
        curl_slist_free_all(headers);
        if (res != CURLE_OK) {
            std::cerr << "Error performing HTTP request: " << curl_easy_strerror(res) << std::endl;
        } else {
            // std::cout << response_data << std::endl;
            char *rok = strtok((char *)response_data.c_str(), ",");
            std::map<std::string, std::string> m;
            while(rok != NULL)
            {
                // std::cout << rok << std::endl;
                splitoutput(rok, m);
                rok = strtok(NULL, ",");
            }
            for (std::map<std::string, std::string>::iterator it = m.begin(); it != m.end(); it++)
            {
                if (it->first == "id" || it->first == "{login" || it->first == "name" || it->first == "avatar_url" || it->first == "bio" || it->first == "location")
                    std::cout << it->first << " : " << it->second << std::endl;
            }
            // // Parse and display the response
            // char *rok = strtok((char *)response_data.c_str(), ",");
            // std::map<std::string, std::string> m;
            // while(rok != NULL)
            // {
            //     splitoutput(rok, m);
            //     rok = strtok(NULL, ",");
            // }
        }

        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Error creating curl handle." << std::endl;
    }

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