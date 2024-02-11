
#include <iostream>
#include <fstream>
#include <vector>

std::string getnokta(std::vector<std::string> &vnokat, int size)
{
    std::srand(static_cast<unsigned int>(std::time(NULL)));
    return vnokat[std::rand() % size];
}
std::vector<std::string> getnokat(char *filename)
{
    std::vector<std::string> vnokat;
    std::string line;
    std::ifstream file(filename);
    if (file.is_open())
    {
        while (std::getline(file, line))
            vnokat.push_back(line);
        file.close();
    }
    return vnokat;
}

void nokta(std::string nick, std::vector<std::string> &vnokat,std::string &filename)
{
    vnokat = getnokat(filename.c_str());
    std::cout << "\n* nokta \n " << getnokta(vnokat, vnokat.size()) << std::endl;
}
int main(int ac, char **av)
{
    std::vector<std::string> vnokat = getnokat(av[1]);
    std::cout << "\n* nokta \n " << getnokta(vnokat, vnokat.size()) << std::endl;
    return 0;
}