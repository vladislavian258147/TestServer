#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iomanip>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "23" 
#define DEFAULT_BUFLEN 512 

int main() {
    setlocale(LC_ALL, "rus");
    // Инициализация Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    // Создание сокета
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Создание адреса для привязки
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.S_un.S_addr = INADDR_ANY;
    serverAddress.sin_port = htons(atoi(DEFAULT_PORT));

    // Привязка сокета к адресу
    iResult = bind(listenSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
    if (iResult == SOCKET_ERROR) {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Прослушивание на сокете
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Сервер запущен на порту " << DEFAULT_PORT << std::endl;

    // Прием соединения
    SOCKET clientSocket;
    sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    clientSocket = accept(listenSocket, (sockaddr*)&clientAddress, &clientAddressSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Соединение установлено" << std::endl;

    // Прием данных
    char recvbuf[DEFAULT_BUFLEN];
    unsigned int recvbuflen = DEFAULT_BUFLEN;
    iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        std::cout << "Принято " << iResult << " байт" << std::endl;
        std::cout << "Полученные данные: ";
        for (int i = 0; i < iResult; ++i) {
            std::cout << std::hex << static_cast<unsigned int>(recvbuf[i] < 0 ? recvbuf[i] + 256 : recvbuf[i]) << " ";
        }
        std::cout << std::endl;
    }
    else if (iResult == 0) {
        std::cout << "Соединение закрыто" << std::endl;
    }
    else {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
    }

    // Закрытие сокетов
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}