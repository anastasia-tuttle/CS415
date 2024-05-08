//
//  sever.cpp
//  TuttleExam2Q4
//
//  Created by Anastasia Tuttle  on 3/21/24.
//

#include "server.hpp"

#include <iostream>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // Add this header for inet_pton()

#define PORT 5970
#define BUFFER_SIZE 1024

using namespace std;

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 5970
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 5970
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    cout << "Server listening on port " << PORT << endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread > 0) {
            // Parse client's message
            char* token = strtok(buffer, ",");
            char msgType = token[0];
            token = strtok(NULL, ",");
            long clientTime = atol(token);

            // Get server's time
            time_t currentTime = time(NULL);
            long serverTime = static_cast<long>(currentTime);

            // Calculate the time difference
            long timeDiff = serverTime - clientTime;

            // Prepare response
            string response;
            if (abs(timeDiff) > 5) {
                response += "X,"; // Update notice
                response += to_string(serverTime); // Server time
            } else {
                response += "C,"; // Update notice
                response += to_string(serverTime); // Server time
            }

            // Send response to client
            send(new_socket, response.c_str(), response.length(), 0);
        }
        close(new_socket);
    }
    return 0;
}
