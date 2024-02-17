#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cstring>
using namespace std;

string getChannel(string cmd)
{
    for (int i = 0; i < cmd.size(); i++)
    {
        if (cmd[i] == '#')
        {
            int j = i;
            while (cmd[j] != ' ')
            {
                j++;
            }
            return cmd.substr(i, j - i);
        }
    }
    return "";
}
string topic(string cmd)
{
    for (int i = 0; i < cmd.size(); i++)
    {
        if (cmd[i] == ':')
        {
            return cmd.substr(i);
        }
    }
    return "";
}

// #include <vector>

#include <iostream>
#include <sstream>
#include <vector>
using namespace std;


int main() {
    std::string input = "TOPIC #te:st:  : Hello World";

    int i = 0;
    std::vector<std::string> scmd;

    for (int i = 0; i < input.size(); i++)
    {
        if (input[i] == 32 && input[i + 1] == ':')
        {
            // scmd.push_back(input.substr(0, i));
            scmd.push_back(input.substr(i + 1));
            break;
        }
    }

    cout<< "size: " << scmd.size() << endl;
    cout  << "cmd: " << scmd[0] << endl;
    cout << "topic: " << scmd[1] << endl;

    return 0;
}


