//
//  client.cpp
//  TuttleExam2Q4
//
//  Created by Anastasia Tuttle  on 3/21/24.
//

#include "client.hpp"

#include <iostream>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5970
#define BUFFER_SIZE 1024

using namespace std;

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "\n Socket creation error \n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        cout << "\nInvalid address/ Address not supported \n";
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "\nConnection Failed \n";
        return -1;
    }

    // Get client's time
    time_t currentTime = time(NULL);
    long clientTime = static_cast<long>(currentTime);

    // Prepare message
    string message = "T," + to_string(clientTime);

    // Send message to server
    send(sock, message.c_str(), message.length(), 0);
    cout << "Message sent to server." << endl;

    // Receive response from server
    valread = read(sock, buffer, BUFFER_SIZE);
    if (valread > 0) {
        // Parse server's response
        char* token = strtok(buffer, ",");
        char msgType = token[0];
        token = strtok(NULL, ",");
        long serverTime = atol(token);

        // Output the server time
        cout << "Server Time: " << serverTime << endl;
    }

    return 0;
}
