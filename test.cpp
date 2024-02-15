
#include <iostream>
#include <vector>
#include <sstream>

std::vector<std::string> parseCommand(const std::string& command) {
    std::vector<std::string> result;
    std::istringstream iss(command);
    std::string token;

    // Parse the command by spaces
    while (std::getline(iss, token, ' ')) {
        if (token.empty()) {
            continue;  // Skip empty tokens
        }

        // Check if the token contains ':'
        size_t colonPos = token.find(':');
        if (colonPos != std::string::npos) {
            result.push_back(token.substr(0, colonPos + 1));
            result.push_back(token.substr(colonPos + 1));
        } else
        {
            result.push_back(token);
        }
    }

    return result;
}

int main() {
    std::string inputCommand = "topic #channel  :   dsd  ds ds ds";
    std::vector<std::string> result = parseCommand(inputCommand);

    // Display the result
    for (int i = 0; i < result.size(); i++) {
        std::cout << result[i];
    }

    return 0;
}