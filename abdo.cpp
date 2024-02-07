#include <iostream>
#include <ctime>

// int main() {
//     std::tm date;
//     std::memset(&date, 0, sizeof(date));

//     // Adjustments for zero-based month and 1900-based year
//     date.tm_year = 1997 - 1900;
//     date.tm_mon = 8;  // September
//     date.tm_mday = 25;

//     std::time_t age_in_sec = std::mktime(&date);
//     std::time_t current_time;
//     std::time(&current_time);

//     // Calculate age using the correct approximation (365.25)
//     int age_in_years = ((current_time - age_in_sec) / (365.25 * 24 * 60 * 60));

//     std::cout << "Age: " << age_in_years << " years" << std::endl;

//     return 0;
// }


#include "Bot.hpp"
#include <ctime>


int main()
{
    std::tm date;
    memset(&date, 0, sizeof(date));
    date.tm_year = 1997 - 1900;
    date.tm_mon  = 7;
    date.tm_mday = 25;

    std::time_t age_in_sec = mktime(&date);
    std::time_t current_time;
    std::time(&current_time);
    std::cout << "age: " << ((current_time - age_in_sec) / (365.25 * 24 * 60 * 60)) << std::endl;
}