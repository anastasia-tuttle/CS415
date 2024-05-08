//
//  client.cpp
//  TuttleAsg4
//
//  Created by Anastasia Tuttle  on 2/28/24.
//

#include "client.hpp"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

#define PORT 9876

int main() {
    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Error creating socket\n";
        return 1;
    }

    // Set up server address structure
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Error connecting to server\n";
        close(clientSocket);
        return 1;
    }

    // Receive the fortune from the server
    char buffer[1024];
    recv(clientSocket, buffer, sizeof(buffer), 0);

    // Display the fortune
    cout << "Fortune: " << buffer << endl;

    // Close the client socket
    close(clientSocket);

    return 0;
}
