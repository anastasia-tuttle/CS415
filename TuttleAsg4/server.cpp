//
//  server.cpp
//  TuttleAsg4
//
//  Created by Anastasia Tuttle  on 2/28/24.
//

#include "server.hpp"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

#define PORT 9876
#define MAX_CONNECTIONS 5

int main() {
    // Seed for random number generation
    srand(time(nullptr));

    // Create a vector to store fortunes
    vector<string> fortunes;

    // Add fortunes using push_back
    fortunes.push_back("You will have a great day!");
    fortunes.push_back("Expect a pleasant surprise!");
    fortunes.push_back("Adventure awaits you!");
    fortunes.push_back("Good things come to those who wait.");
    fortunes.push_back("Be open to new opportunities.");
    fortunes.push_back("A journey of a thousand miles begins with a single step.");

    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Error creating socket\n";
        return 1;
    }

    // Set up server address structure
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Error binding socket\n";
        close(serverSocket);
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CONNECTIONS) == -1) {
        cerr << "Error listening for connections\n";
        close(serverSocket);
        return 1;
    }

    cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        // Accept a connection
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            cerr << "Error accepting connection\n";
            close(serverSocket);
            return 1;
        }

        // Randomly select a fortune
        int randomIndex = std::rand() % fortunes.size();
        const char* fortune = fortunes[randomIndex].c_str();

        // Send the fortune to the client
        send(clientSocket, fortune, strlen(fortune), 0);

        // Close the client socket
        close(clientSocket);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
