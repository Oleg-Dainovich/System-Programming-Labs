#include <iostream>
#include <winsock2.h>
#include <thread>
#include <string>

#pragma comment(lib, "WS2_32.lib")

// Буфер для чтения и записи данных
const int BUFFER_SIZE = 4096;
char buffer[BUFFER_SIZE];

void ReceiveMessages(SOCKET clientSocket) {
    while (true) {
        int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        }
        std::cout << "Client: " << buffer << std::endl;

        // Очистка буфера
        memset(buffer, 0, BUFFER_SIZE);
    }
}

void SendMessages(SOCKET clientSocket) {
    while (true) {
        std::string message;
        std::getline(std::cin, message);

        if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
            std::cout << "Failed to send data to the client." << std::endl;
            break;
        }
    }
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;

    // Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Failed to initialize socket." << std::endl;
        return 1;
    }

    // Создание сокета
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cout << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Настройка адреса сервера
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);  // Порт сервера
    serverAddress.sin_addr.s_addr = INADDR_ANY;  // Принимать подключения на все IP-адреса

    // Привязка сокета к адресу сервера
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cout << "Failed to bind socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Прослушивание входящих подключений
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Failed to listen on socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for connections..." << std::endl;

    // Принятие входящих подключений
    int clientAddressSize = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Failed to accept client connection." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Client connected." << std::endl;

    std::thread receiveThread(ReceiveMessages, clientSocket);
    std::thread sendThread(SendMessages, clientSocket);

    receiveThread.join();
    sendThread.join();

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
