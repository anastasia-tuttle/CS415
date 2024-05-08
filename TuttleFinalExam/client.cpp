//
//  client.cpp
//  TuttleFinalExam
//
//  Created by Anastasia Tuttle  on 4/24/24.
//

#include "client.hpp"

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

constexpr int PORT = 6982;
constexpr int MAX_MSG_SIZE = 1024; // Maximum message size

class Logger {
private:
    int clientSocket;

public:
    Logger() {
        // Create socket
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            std::cerr << "Error creating socket" << std::endl;
        }

        // Server address structure
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change to your server's IP
        serverAddr.sin_port = htons(PORT);

        // Connect to server
        if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
            std::cerr << "Error connecting to server" << std::endl;
            close(clientSocket);
        }
    }

    ~Logger() {
        // Close socket
        close(clientSocket);
    }

    void log(const std::string& errorlevel, const std::string& sourceid, const std::string& message) {
        std::string logMessage = errorlevel + "," + sourceid + "," + message;

        // Send message to server
        send(clientSocket, logMessage.c_str(), logMessage.size(), 0);
    }

    void urgent(const std::string& sourceid, const std::string& message) {
        log("urgent", sourceid, message);
    }

    void critical(const std::string& sourceid, const std::string& message) {
        log("critical", sourceid, message);
    }

    void error(const std::string& sourceid, const std::string& message) {
        log("error", sourceid, message);
    }

    void warning(const std::string& sourceid, const std::string& message) {
        log("warning", sourceid, message);
    }
};

int main() {
    Logger logger;

    // Test logging messages
    logger.urgent("source1", "This is an urgent message");
    logger.critical("source2", "This is a critical message");
    logger.error("source3", "This is an error message");
    logger.warning("source4", "This is a warning message");

    return 0;
}
