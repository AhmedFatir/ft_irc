#include <iostream>
#include <curl/curl.h>
#include <map>
#include <iostream>
#include <curl/curl.h>
#include <map>

#include <iostream>
#include <string>

#include <iostream>

std::string getpickline(const std::string pickuplines[], int size)
{
    std::srand(static_cast<unsigned int>(std::time(NULL)));
    int index = std::rand() % size;
    return pickuplines[index];
}

int main()
{
    const std::string pickuplines[] = {
        "When I look in your eyes, I see a very kind soul.",
        "Do you happen to have a Band-Aid? ‘Cause I scraped my knees falling for you.",
        "I never believed in love at first sight, but that was before I saw you.",
        "f being sexy was a crime, you’d be guilty as charged.",
        "Do you have a map? I just got lost in your eyes.",
        "I would never play hide and seek with you because someone like you is impossible to find.",
        "Want to go outside and get some fresh air with me? You just took my breath away.",
        "Is your name Google? Because you have everything I’m searching for.",
        "I must be a snowflake because I’ve fallen for you.",
        "I’m not a photographer, but I can definitely picture us together.",
        "I’m not a genie, but I can make your dreams come true.",
        "You know, your smile has been lighting up the room all night and I just had to come and say hello.",
        "I can’t tell if that was an earthquake, or if you just seriously rocked my world.",
        };
    std::string random_pickupline = getpickline(pickuplines, sizeof(pickuplines) / sizeof(pickuplines[0]));
    std::cout << "\n[💓💓 " << random_pickupline << " 💓💓]" << std::endl;
    return 0;
}