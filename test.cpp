
#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;


struct MemoryStruct {
  char *memory;
  size_t size;
};
typedef struct  {
  char *memory;
  size_t size;
} Response;

// static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
// {
//   size_t realsize = size * nmemb;
//   struct MemoryStruct *mem = (struct MemoryStruct *)userp;

//   char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
//   if(ptr == NULL) {
//     /* out of memory! */
//     printf("not enough memory (realloc returned NULL)\n");
//     return 0;
//   }

//   mem->memory = ptr;
//   memcpy(&(mem->memory[mem->size]), contents, realsize);
//   mem->size += realsize;
//   mem->memory[mem->size] = 0;

//   return realsize;
// }

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    puts("\n\n BEGIN CHUNK \n\n");
    for (int i = 0; i < size * nmemb; i++)
    {
        printf("%c", ((char *)contents)[i]);
    }
    puts("\n\n END CHUNK \n\n");
    return nmemb;
}
int main() {
   
   CURL *curl;
   CURLcode res;

   struct MemoryStruct chunk;
    chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
   
   curl = curl_easy_init();

   if (!curl)
   {
        cerr << "Error: curl_easy_init() failed" << endl;
        return -1;
   }
   Response response;
   
   curl_easy_setopt(curl, CURLOPT_URL, "https://www.google.com");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
   res = curl_easy_perform(curl);

   if (res != CURLE_OK)
   {
        cerr << "Error: curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        return -1;
   }
   cout << "res ----- > " << chunk.memory << endl;
   free(chunk.memory);
   curl_easy_cleanup(curl);
    return 0;
}
